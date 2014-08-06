#include <libtorrent/session.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>

#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"

using namespace std;

gt::Core::Core(int argc, char **argv) :
	m_running(true)
{

	if(!gt::Platform::processIsUnique())
	{
		gt::Log::Debug("An instance is already running");
		gt::Platform::writeSharedData(argv[1]);
		exit(0);
	}

	gt::Platform::makeSharedFile();

	// Fuck your deprecated shit, we're going void down in here
	// tl;dr, figure out something useful to use the error code for,
	// like handling what the fuck might happen if listen_on fails kthnx
	loadSession(gt::Platform::getDefaultConfigPath());

	libtorrent::error_code ec;
	m_session.listen_on(make_pair(6881, 6889), ec, (const char *)0, 0); //ambigous between new and deprecated function
	if (ec.value() != 0)
		gt::Log::Debug(ec.message().c_str());

	for(int i = 1; i < argc; ++i)
		addTorrent(string(argv[i]));

    statuses.update(&m_torrents);
}

bool gt::Core::isMagnetLink(string const& url)
{
	const string prefix = "magnet:";
	return url.compare(0, prefix.length(), prefix) == 0;
}

shared_ptr<gt::Torrent> gt::Core::addTorrent(string path, vector<char> *resumedata)
{
	if (path.empty())
		return shared_ptr<gt::Torrent>();//Use default constructor instead of null

	shared_ptr<gt::Torrent> t;

	try
	{
		t = make_shared<gt::Torrent>(path);
		//pointer necessary to catch exception as a shared ptr would go out of scope
	}
	catch (int exception)
	{
		return shared_ptr<gt::Torrent>();
		//Return null if invalid torrent to be handled by GtkMainWindow
	}

	libtorrent::error_code ec;
	auto params = t->getTorrentParams();
	params.resume_data = resumedata != nullptr ? *resumedata : vector<char>(); //TODO: Look if fast resume data exists for this torrent
	libtorrent::torrent_handle h = m_session.add_torrent(params, ec);

	//Actually, libtorrent silentely deals with duplicates, we just have to make this function not to return another Torrent to the UI
	for(auto tor : getTorrents())
		if(t->getHandle().status().has_metadata && tor->getHandle().info_hash() == t->getTorrentParams().ti->info_hash())
			return shared_ptr<gt::Torrent>();

	if (ec.value() != 0)
	{
		gt::Log::Debug(ec.message().c_str());
		return shared_ptr<gt::Torrent>();
	}
	else
	{
		t->setHandle(h);
		m_torrents.push_back(t);
		if(t->hasMetadata() && gt::Settings::settings["DefaultSequentialDownloading"] == "Yes")
		{
			if(t->filenames().size() == 1)
			{
				string ext = t->filenames()[0].substr(t->filenames()[0].find_last_of('.') + 1);
				t->setSequentialDownload(gt::Settings::settings["SequentialDownloadExtensions"].find(ext) != string::npos);
			}
		}
        statuses.update(&m_torrents);
		return t;
	}
}

void gt::Core::removeTorrent(shared_ptr<Torrent> t)
{
	//TODO : add removal of files on request
	//TODO : Remove fast resume data associated to file
	m_session.remove_torrent(t->getHandle());
	unsigned i;
	for(i = 0; i < m_torrents.size(); ++i)
		if(m_torrents[i] == t)
			break;
	while(i < m_torrents.size() - 1)
	{
		m_torrents[i] = m_torrents[i + 1];
		++i;
	}
	m_torrents.resize(m_torrents.size() - 1);
    statuses.update(&m_torrents);
}

/*
 * Where path is relative to the executable
 */
int gt::Core::saveSession(string folder)
{
	libtorrent::entry ent;
	m_session.pause();
	m_session.save_state(ent);

	if(!gt::Platform::checkDirExist(folder))
		gt::Platform::makeDir(folder, 0755);


	if(!gt::Platform::checkDirExist(folder + "meta/"))
		gt::Platform::makeDir(folder + "meta/", 0755);

	ofstream state(folder + "state.gts");
	ofstream list(folder + "list.gts");

	if(!state)
		throw "Couldn't open state.gts";

	if(!list)
		throw "Couldn't open list.gts";

	bencode(ostream_iterator<char>(state), ent);
	state.close();

	int count = 0;

	for(auto tor : m_torrents)
	{
		if(!tor->getHandle().is_valid()) continue;
		if(!tor->getHandle().status().has_metadata) continue;
		if(!tor->getHandle().need_save_resume_data()) continue;

		auto ent = libtorrent::create_torrent(*tor->getInfo()).generate();
		ofstream out((folder + "meta/" + tor->getName() + ".torrent").c_str(), std::ios_base::binary);
		out.unsetf(ios_base::skipws);
		bencode(ostream_iterator<char>(out), ent);

		tor->getHandle().save_resume_data();
		++count;
	}

	while(count)
	{
		libtorrent::alert const *al = m_session.wait_for_alert(libtorrent::seconds(10));
		unique_ptr<libtorrent::alert> holder = m_session.pop_alert();

		switch (al->type())
		{
		case libtorrent::save_resume_data_alert::alert_type:
			break;
		case libtorrent::save_resume_data_failed_alert::alert_type:
			gt::Log::Debug("Failed to create resume data. Skipping.");
			--count;
			continue;
		default:
			gt::Log::Debug("Received alert wasn't about resume data. Skipping.");
			continue;
		}

		libtorrent::save_resume_data_alert *rd = (libtorrent::save_resume_data_alert*)al;
		libtorrent::torrent_handle h = rd->handle;
		ofstream out((folder + "meta/" + h.status().name + ".fastresume").c_str(), std::ios_base::binary);
		out.unsetf(ios_base::skipws);
		list << h.status().name << '\n';
		bencode(ostream_iterator<char>(out), *rd->resume_data);
		--count;
	}

	list.close();

	return 0;
}

int gt::Core::loadSession(string folder)
{
	libtorrent::lazy_entry ent;
	libtorrent::error_code ec;

	gt::Settings::parse("config");

	if (!gt::Platform::checkDirExist(folder)               ||
		!gt::Platform::checkDirExist(folder + "state.gts") ||
		!gt::Platform::checkDirExist(folder + "list.gts"))
	{
		// Also creates an empty session.
		gt::Log::Debug(string("Creating new session folder in: " + gt::Platform::getDefaultConfigPath()).c_str());
		saveSession(folder);
	}

	fstream state;
	fstream list;

	state.open(folder + "state.gts");
	list.open(folder + "list.gts");

	if(!state.is_open())
		throw "Couldn't open state.gts";

	if(!list.is_open())
		throw "Couldn't open list.gts";

	string benfile, tmp;

	do
	{
		getline(state, tmp);
		benfile += tmp;
	}
	while(state);

	lazy_bdecode(benfile.c_str(), benfile.c_str() + benfile.size(), ent, ec);
	m_session.load_state(ent);
	setSessionParameters();
	while(getline(list, tmp))
	{
		if(!gt::Platform::checkDirExist(folder + "meta/" + tmp + ".torrent")) continue; //eventually delete the associated .fasteresume
		libtorrent::add_torrent_params params;
		vector<char> resumebuff;
		ifstream resumedata(folder + "meta/" + tmp + ".fastresume");
		while(resumedata)
			resumebuff.push_back(resumedata.get());
		auto t = addTorrent(folder + "meta/" + tmp + ".torrent", &resumebuff);
	}

	m_session.resume();

	return 0;
}

shared_ptr<gt::Torrent> gt::Core::update()
{
	string str = gt::Platform::readSharedData();
	if(!str.empty()) gt::Log::Debug(str.c_str());
	return addTorrent(str);
}

//TODO: Catch some signals to make sure this function is called
void gt::Core::shutdown()
{
	gt::Log::Debug("Shutting down core library...");
	gt::Platform::disableSharedData();
	saveSession(gt::Platform::getDefaultConfigPath());
	gt::Settings::save("config");
	m_running = false;
}


void gt::Core::setSessionParameters()
{
	using namespace gt;
	libtorrent::session_settings se = m_session.settings();

	if(Settings::settings["OverrideSettings"] != "No")
	{
		if(Settings::settings["OverrideSettings"] == "Minimal")
			se = libtorrent::min_memory_usage();
		else if(Settings::settings["OverrideSettings"] == "HighPerformanceSeeding")
			se = libtorrent::high_performance_seed();
	}

	if(Settings::settings["ProxyHost"] != "")
	{
		libtorrent::proxy_settings pe;
		string user, pass;
		pe.hostname = Settings::settings["ProxyHost"];
		if(Settings::settings["ProxyCredentials"] != "")
		{
			user = Settings::settings["ProxyCredentials"].substr(0, Settings::settings["ProxyCredentials"].find(':'));
			pass = Settings::settings["ProxyCredentials"].substr(Settings::settings["ProxyCredentials"].find(':'), string::npos);
		}
		pe.username = user;

		if(Settings::settings["ProxyType"] == "HTTP")
			pe.type = 4;
		else if(Settings::settings["ProxyType"] == "SOCKS5")
			pe.type = 2;
		else if(Settings::settings["ProxyType"] == "SOCKS4")
			pe.type = 1;
		else
			pe.type = 0;

		pe.type += pass != "";
		pe.password = pass;
		m_session.set_proxy(pe);
	}

	try
	{
		if(stoi(Settings::settings["CacheSize"]) > 0) se.cache_size = stoi(Settings::settings["CacheSize"]);
		if(stoi(Settings::settings["CachedChunks"]) > 0) se.cache_buffer_chunk_size = stoi(Settings::settings["CachedChunks"]);
		if(stoi(Settings::settings["CacheExpiry"]) > 0) se.cache_expiry = stoi(Settings::settings["CacheExpiry"]);
	}
	catch(...)
	{}

	if(Settings::settings["AnonymousMode"] == "Yes") se.anonymous_mode = true;
	if(Settings::settings["ChokingAlgorithm"] != "Default")
	{
		if(Settings::settings["ChokingAlgorithm"] == "AutoExpand") se.choking_algorithm = 1;
		else if(Settings::settings["ChokingAlgorithm"] == "RateBased") se.choking_algorithm = 2;
		else if(Settings::settings["ChokingAlgorithm"] == "BitTyrant")
		{
			se.choking_algorithm = 3;
			try
			{
				if(stoi(Settings::settings["DefaultReciprocationRate"]) > 0) se.default_est_reciprocation_rate = stoi(Settings::settings["DefaultReciprocationRate"]);
				if(stoi(Settings::settings["IncreaseReciprocationRate"]) > 0) se.increase_est_reciprocation_rate = stoi(Settings::settings["IncreaseReciprocationRate"]);
				if(stoi(Settings::settings["DecreaseReciprocationRate"]) > 0) se.decrease_est_reciprocation_rate = stoi(Settings::settings["DecreaseReciprocationRate"]);
			}
			catch(...) {}
		}
		else se.choking_algorithm = 0;
	}
	if(Settings::settings["SeedChokingAlgorithm"] != "RoundRobin")
	{
		if(Settings::settings["SeedChokingAlgorithm"] == "FastestUpload") se.seed_choking_algorithm = 1;
		else if(Settings::settings["SeedChokingAlgorithm"] == "AntiLeech") se.seed_choking_algorithm = 2;
		else se.seed_choking_algorithm = 0;
	}

	se.user_agent = Settings::settings["UserAgent"];
	if(Settings::settings["PieceSuggestion"] == "No") se.suggest_mode = 0;

	try
	{
		if(stoi(Settings::settings["GlobalUploadLimit"]) > 0) se.upload_rate_limit = stoi(Settings::settings["GlobalUploadLimit"]);
		if(stoi(Settings::settings["DHTUploadLimit"]) > 0) se.dht_upload_rate_limit = stoi(Settings::settings["DHTUploadLimit"]);
		if(stoi(Settings::settings["GlobalDownloadLimit"]) > 0) se.download_rate_limit = stoi(Settings::settings["GlobalDownloadLimit"]);
	}
	catch(...)
	{}

	if(Settings::settings["ReportTrueDownloaded"] == "Yes") se.report_redundant_bytes = true;
/*	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);
	if(Settings::settings[""]);*/
	m_session.set_settings(se);
}

int gt::Core::statusList::update(std::vector<std::shared_ptr<Torrent>> *tl) {
    downloading.clear();
    seeding.clear();
    checking.clear();
    stopped.clear();
    finished.clear();
    for(int i = 0; i < tl->size(); i++) {
        if(tl->at(i)->getState() == libtorrent::torrent_status::state_t::downloading) {
            downloading.push_back(tl->at(i));
            continue;
        }
        if(tl->at(i)->getState() == libtorrent::torrent_status::state_t::seeding) {
            seeding.push_back(tl->at(i));
            continue;
        }
        if((tl->at(i)->getState() == libtorrent::torrent_status::state_t::checking_files)||(tl->at(i)->getState() == libtorrent::torrent_status::state_t::checking_resume_data)) {
            checking.push_back(tl->at(i));
            continue;
        }
        if(tl->at(i)->getState() == libtorrent::torrent_status::state_t::finished) {
            finished.push_back(tl->at(i));
            continue;
        }
        /* if(tl->at(i).getState() == libtorrent::torrent_status::state_t::paused) {
            paused.push_back(tl->at(i));
        } */
        stopped.push_back(tl->at(i));
    }
    return 1;
}

gt::Core::statusList* gt::Core::getStatuses() {
    return &statuses;
}



