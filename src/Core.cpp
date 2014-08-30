#include <libtorrent/session.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>

#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"
#include <Feed.hpp>

// TODO: Restore RSS feeds on load with their default signal handlers
// TODO: Create RSS specific settings
// TODO: Also I think I forgot to implement something important but can't remember what.
// TODO: Maybe related to the line above: add a blocking method in Feed to block control until feed is up to date ?

gt::Core::Core(int argc, char **argv) :
	m_session(libtorrent::fingerprint("GT", 0, 0, 2, 0), 0, 0x7FFFFFFF),
	m_running(true)
{
	if(!gt::Platform::processIsUnique())
	{
		gt::Log::Debug("An instance is already running");
		if (argc > 1)
			gt::Platform::writeSharedData(argv[1]);
		exit(0);
	}

	try
	{
		gt::Platform::makeSharedFile();
	}
	catch(std::runtime_error msfError)
	{
		gt::Log::Debug(msfError.what());
	}

	loadSession(gt::Platform::getDefaultConfigPath());

	libtorrent::error_code ec;
	m_session.listen_on(std::make_pair(6881, 6889), ec, (const char *)0, 0);
	if (ec.value() != 0)
		gt::Log::Debug(ec.message().c_str());

	for(int i = 1; i < argc; ++i)
		addTorrent(std::string(argv[i]));
	statuses.update(&m_torrents);
}

std::vector<std::shared_ptr<gt::Torrent>> &gt::Core::getTorrents()
{
	return m_torrents;
}

bool gt::Core::isLink(std::string const& url)
{
	const std::string magprefix = "magnet:", httpprefix = "http:";
	return url.compare(0, magprefix.length(), magprefix) == 0 || url.compare(0, httpprefix.length(), httpprefix) == 0;
}

std::shared_ptr<gt::Torrent> gt::Core::addTorrent(std::string path, std::vector<char> *resumedata)
{
	if (path.empty())
		return std::shared_ptr<gt::Torrent>();//Use default constructor instead of null

	std::shared_ptr<gt::Torrent> t;

	try
	{
		t = std::make_shared<gt::Torrent>(path);
		//pointer necessary to catch exception as a shared ptr would go out of scope
	}
	catch (int exception)
	{
		return std::shared_ptr<gt::Torrent>();
		//Return null if invalid torrent to be handled by GtkMainWindow
	}

	libtorrent::error_code ec;
	auto params = t->getTorrentParams();
	params.flags |= 128; // torrent duplication is error
	params.resume_data = resumedata != nullptr ? *resumedata : std::vector<char>(); //TODO: Look if fast resume data exists for this torrent
	libtorrent::torrent_handle h = m_session.add_torrent(params, ec);

	if(!h.status().has_metadata)
		for(auto tor : getTorrents())
			if(t->getTorrentParams().url == tor->getTorrentParams().url)
				return std::shared_ptr<gt::Torrent>();

	if (ec.value() != 0)
	{
		gt::Log::Debug(ec.message().c_str());
		return std::shared_ptr<gt::Torrent>();
	}
	else
	{
		*t = h;
		if(gt::Settings::settings["DHTEnabled"] == "Yes")
			h.force_dht_announce();

		m_torrents.push_back(t);
		if(t->status().has_metadata && gt::Settings::settings["DefaultSequentialDownloading"] == "Yes")
			if(t->filenames().size() == 1)
			{
				std::string ext = t->filenames()[0].substr(t->filenames()[0].find_last_of('.') + 1);
				t->set_sequential_download(gt::Settings::settings["SequentialDownloadExtensions"].find(ext) != std::string::npos);
			}
		statuses.update(&m_torrents);
		return t;
	}
}

void gt::Core::removeTorrent(std::shared_ptr<Torrent> t)
{
	//TODO : add removal of files on request
	//TODO : Remove fast resume data associated to file
	m_session.remove_torrent(libtorrent::torrent_handle(*t)); //explicit cast is required to make a copy of the underlying object
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

bool gt::Core::isRunning() const
{
	return m_running;
}

/*
 * Where path is relative to the executable
 */
int gt::Core::saveSession(std::string folder)
{
	libtorrent::entry ent;
	m_session.pause();
	m_session.save_state(ent);

	if(!gt::Platform::checkDirExist(folder))
		gt::Platform::makeDir(folder, 0755);

	if(!gt::Platform::checkDirExist(folder + "meta/"))
		gt::Platform::makeDir(folder + "meta/", 0755);

	std::ofstream state(folder + "state.gts");
	std::ofstream list(folder + "list.gts");

	if(!state)
		throw std::runtime_error("Couldn't open state.gts");

	if(!list)
		throw std::runtime_error("Couldn't open list.gts");

	bencode(std::ostream_iterator<char>(state), ent);
	state.close();

	int count = 0;

	for(auto tor : m_torrents)
	{
		if(!tor->is_valid()) continue;
		if(!tor->status().has_metadata) continue;
		if(!tor->need_save_resume_data()) continue;

		auto ent = libtorrent::create_torrent(*tor->torrent_file()).generate();
		std::ofstream out((folder + "meta/" + tor->status().name + ".torrent").c_str(), std::ios_base::binary);
		out.unsetf(std::ios_base::skipws);
		bencode(std::ostream_iterator<char>(out), ent);

		tor->save_resume_data();
		++count;
	}

	while(count)
	{
		libtorrent::alert const *al = m_session.wait_for_alert(libtorrent::seconds(10));
		std::unique_ptr<libtorrent::alert> holder = m_session.pop_alert();

		switch (al->type())
		{
		case libtorrent::save_resume_data_alert::alert_type:
			break;
		case libtorrent::save_resume_data_failed_alert::alert_type:
			gt::Log::Debug("Failed to create resume data. Skipping.");
			--count;
		default:
			continue;
		}

		libtorrent::save_resume_data_alert *rd = (libtorrent::save_resume_data_alert*)al;
		libtorrent::torrent_handle h = rd->handle;
		std::ofstream out((folder + "meta/" + h.status().name + ".fastresume").c_str(), std::ios_base::binary);
		out.unsetf(std::ios_base::skipws);
		list << h.status().name << '\n';
		bencode(std::ostream_iterator<char>(out), *rd->resume_data);
		--count;
	}

	list.close();

	return 0;
}

int gt::Core::loadSession(std::string folder)
{
	libtorrent::lazy_entry ent;
	libtorrent::error_code ec;

	gt::Settings::parse("config");

	if (!gt::Platform::checkDirExist(folder)                   ||
	        !gt::Platform::checkDirExist(folder + "state.gts") ||
	        !gt::Platform::checkDirExist(folder + "list.gts"))
	{
		// Also creates an empty session.
		gt::Log::Debug(std::string("Creating new session folder in: " + gt::Platform::getDefaultConfigPath()).c_str());
		saveSession(folder);
	}

	std::fstream state;
	std::fstream list;

	state.open(folder + "state.gts");
	list.open(folder + "list.gts");

	if(!state.is_open())
		throw "Couldn't open state.gts";

	if(!list.is_open())
		throw "Couldn't open list.gts";

	std::string benfile, tmp;

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
		std::vector<char> resumebuff;
		std::ifstream resumedata(folder + "meta/" + tmp + ".fastresume");
		while(resumedata)
			resumebuff.push_back(resumedata.get());
		auto t = addTorrent(folder + "meta/" + tmp + ".torrent", &resumebuff);
	}

	m_session.resume();
	m_session.set_alert_mask(0x7FFFFFFF);
	return 0;
}

/*
	Feed f("http://feed.url", this);
	auto lam = [](std::string title) { return title.find("Filter like you want") != std::string::npos; };

	for(auto item : f.getFilteredItems(lam))
		std::cout << item.title << std::endl;

 */
std::shared_ptr<gt::Torrent> gt::Core::update()
{
	std::string str = gt::Platform::readSharedData();
	if(!str.empty()) gt::Log::Debug(str.c_str());

	std::deque<libtorrent::alert*> alerts;
	std::deque<libtorrent::alert*> unhandledAlerts;

	m_session.pop_alerts(&alerts);
	while(!alerts.empty())
	{
		libtorrent::alert *al = alerts[0];
		if(al->type() != libtorrent::state_changed_alert::alert_type) { alerts.pop_front(); unhandledAlerts.push_front(al); continue; } // should fix the incorrect values passed to onStateChanged
		libtorrent::state_changed_alert *scal = static_cast<libtorrent::state_changed_alert *>(al);
		for(auto tor : m_torrents)
			if(*tor == scal->handle)
			{
				tor->onStateChanged(scal->prev_state, tor);
				alerts.pop_front();
				break;
			}
		alerts.pop_front();
	}

	alerts = unhandledAlerts;
	unhandledAlerts = std::deque<libtorrent::alert*>();

	if(m_session.is_dht_running() && gt::Settings::settings["DHTEnabled"] == "Yes")
	{
		while(!alerts.empty())
		{
			libtorrent::alert *al = alerts[0];
			switch(al->type())
			{
			case libtorrent::dht_reply_alert         ::alert_type:         
			case libtorrent::dht_announce_alert      ::alert_type:      
			case libtorrent::dht_get_peers_alert     ::alert_type:     
			case libtorrent::dht_bootstrap_alert     ::alert_type:     
			case libtorrent::dht_error_alert         ::alert_type:         
			case libtorrent::dht_immutable_item_alert::alert_type:
			case libtorrent::dht_mutable_item_alert  ::alert_type:  
			case libtorrent::dht_put_alert           ::alert_type:           
				alerts.pop_front();
				break;
			default:
				alerts.pop_front(); 
				unhandledAlerts.push_front(al);//we won't handle this alert
			}
		}

		/*gt::Log::Debug("\nDHT Upload Rate: "  + std::to_string(m_session.status().dht_upload_rate)          + '\n' +
					   "DHT Download Rate: "  + std::to_string(m_session.status().dht_download_rate)        + '\n' +
					   "DHT Upload Total: "   + std::to_string(m_session.status().total_dht_upload)         + '\n' +
					   "DHT Active Nodes: "   + std::to_string(m_session.status().dht_nodes)                + '\n' +
					   "DHT Total Nodes: "    + std::to_string(m_session.status().dht_global_nodes)         + '\n' +
					   "DHT Total Usage: "    + getFileSizeString(m_session.status().dht_total_allocations) + '\n' +
					   "DHT Download Total: " + std::to_string(m_session.status().total_dht_download)       + '\n');
		*/

		alerts = unhandledAlerts;
		unhandledAlerts = std::deque<libtorrent::alert*>();

		while(!alerts.empty())
		{
			libtorrent::alert *al = alerts[0];
			if(al->type() != libtorrent::rss_alert::alert_type) { alerts.pop_front(); unhandledAlerts.push_front(al); continue; } // should fix the incorrect values passed to onStateChanged
			libtorrent::rss_alert *rssal = static_cast<libtorrent::rss_alert *>(al);
			for(auto feed : m_feeds)
				if(*feed == rssal->handle)
				{
					feed->onStateChanged(rssal->state, feed);
					alerts.pop_front();
					break;
				}
			alerts.pop_front();
		}


		while(!alerts.empty())
		{
			gt::Log::Debug(alerts[0]->message());
			alerts.pop_front(); 
		}
	}
	return addTorrent(str);
}

//TODO: Catch some signals to make sure this function is called
void gt::Core::shutdown()
{
	gt::Log::Debug("Shutting down core library...");
	gt::Platform::disableSharedData();
	gt::Log::Debug("Saving session...");
	saveSession(gt::Platform::getDefaultConfigPath());
	gt::Settings::save("config");

	if(m_session.is_dht_running())
	{
		gt::Log::Debug("Stopping DHT...");
		m_session.stop_dht();
	}
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
		std::string user, pass;
		pe.hostname = Settings::settings["ProxyHost"];
		if(Settings::settings["ProxyCredentials"] != "")
		{
			user = Settings::settings["ProxyCredentials"].substr(0, Settings::settings["ProxyCredentials"].find(':'));
			pass = Settings::settings["ProxyCredentials"].substr(Settings::settings["ProxyCredentials"].find(':'), std::string::npos);
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
		if(stoi(Settings::settings["CacheSize"   ]) > 0) se.cache_size              = stoi(Settings::settings["CacheSize"   ]);
		if(stoi(Settings::settings["CacheExpiry" ]) > 0) se.cache_expiry            = stoi(Settings::settings["CacheExpiry" ]);
		if(stoi(Settings::settings["CachedChunks"]) > 0) se.cache_buffer_chunk_size = stoi(Settings::settings["CachedChunks"]);
	}
	catch(...)
	{}

	if(Settings::settings["AnonymousMode"] == "Yes") se.anonymous_mode = true;
	if(Settings::settings["ChokingAlgorithm"] != "Default")
	{
		if     (Settings::settings["ChokingAlgorithm"] == "AutoExpand") se.choking_algorithm = 1;
		else if(Settings::settings["ChokingAlgorithm"] == "RateBased") se.choking_algorithm = 2;
		else if(Settings::settings["ChokingAlgorithm"] == "BitTyrant")
		{
			se.choking_algorithm = 3;
			try
			{
				if(stoi(Settings::settings["DefaultReciprocationRate" ]) > 0) se.default_est_reciprocation_rate  = stoi(Settings::settings["DefaultReciprocationRate" ]);
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
		se.active_seeds          = stoi(Settings::settings["ActiveSeeds"        ]);
		se.active_downloads      = stoi(Settings::settings["ActiveDownloads"    ]);
		se.dht_upload_rate_limit = stoi(Settings::settings["DHTUploadLimit"     ]);
		se.upload_rate_limit     = stoi(Settings::settings["GlobalUploadLimit"  ]);
		se.download_rate_limit   = stoi(Settings::settings["GlobalDownloadLimit"]);
		se.active_limit = se.active_seeds + se.active_downloads;
	}
	catch(...)
	{}

	if(gt::Settings::settings["DHTEnabled"] == "Yes")
	{
		gt::Log::Debug("Starting DHT...");
		while(!m_session.is_dht_running())
			m_session.start_dht();
		std::string tmp = gt::Settings::settings["DHTBootstraps"];
		while(!tmp.empty())
		{
			gt::Log::Debug("Adding " + tmp.substr(0, tmp.find(',')) + " to the DHT node list...");
			m_session.add_dht_node(std::make_pair(tmp.substr(0, tmp.find(',')), 6881));
			if(tmp.find(',') == std::string::npos) break;
			tmp = tmp.substr(tmp.find(',') + 1);
		}
	}
	else
		while(m_session.is_dht_running())
			m_session.stop_dht();

	if(gt::Settings::settings["LSDEnabled"] == "Yes")
	{
		gt::Log::Debug("Starting LSD...");
		m_session.start_lsd();
	}
	else
		m_session.stop_lsd();

	if(gt::Settings::settings["NATPMPEnabled"] == "Yes")
	{
		gt::Log::Debug("Starting NAT-PMP...");
		m_session.start_natpmp();
		int protNum = 1 + ((gt::Settings::settings["DHTEnabled"] == "Yes") << 1);
		m_session.add_port_mapping(libtorrent::session::protocol_type(protNum), 6881, 6667);
	}
	else
		m_session.stop_natpmp();


	if(gt::Settings::settings["UPnPEnabled"] == "Yes")
	{
		gt::Log::Debug("Starting UPnP...");
		m_session.start_upnp();
		int protNum = 1 + ((gt::Settings::settings["DHTEnabled"] == "Yes") << 1);
		m_session.add_port_mapping(libtorrent::session::protocol_type(protNum), 6881, 6666);
	}
	else
		m_session.stop_upnp();

	se.auto_manage_interval = 1;
	if(Settings::settings["ReportTrueDownloaded"] == "Yes") se.report_redundant_bytes = true;
	m_session.set_settings(se);
}

int gt::Core::statusList::update(std::vector<std::shared_ptr<Torrent>> *tl)
{
	downloading.clear();
	seeding.clear();
	checking.clear();
	stopped.clear();
	finished.clear();
	for(unsigned i = 0; i < tl->size(); i++)
	{
		if(tl->at(i)->isPaused())
		{
			paused.push_back(tl->at(i));
			continue;
		}
		switch(tl->at(i)->status().state)
		{
		case libtorrent::torrent_status::state_t::downloading:
			downloading.push_back(tl->at(i));
			continue;
		case libtorrent::torrent_status::state_t::seeding:
			seeding.push_back(tl->at(i));
			continue;
		case libtorrent::torrent_status::state_t::checking_files:
		case libtorrent::torrent_status::state_t::checking_resume_data:
			checking.push_back(tl->at(i));
			continue;
		case libtorrent::torrent_status::state_t::finished:
			finished.push_back(tl->at(i));
			continue;
		default:
			stopped.push_back(tl->at(i));
		}
	}
	return 1;
}

gt::Core::statusList* gt::Core::getStatuses()
{
	return &statuses;
}


std::shared_ptr<gt::Feed> gt::Core::addFeed(std::string Url)
{
	auto f = std::make_shared<gt::Feed>(Url, this);
	// default handler
	f->onStateChanged = [](int state, std::shared_ptr<gt::Feed> feed)
		{
			switch(state)
			{
			case 0:  feed->onUpdateStarted(feed); break;
			case 1: feed->onUpdateFinished(feed); break;
			default: feed->onUpdateErrored(feed);
			}
		};
	m_feeds.push_back(f);
	return f;
}
