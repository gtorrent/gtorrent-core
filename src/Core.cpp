#include <libtorrent/session.hpp>
#include <libtorrent/lazy_entry.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>

#include "Core.hpp"
#include "Feed.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"
#include "Torrent.hpp"

/*
 * RSS todos
 * DONE: Restore RSS feeds on load with their default signal handlers
 * DONE: Save And reload rss filters
 * TODO: Rerun the filter/add when a filter/function change
 * TODO: Add unary function
 * TODO: Better file parsing
 * TODO: Create RSS specific settings
 * TODO: Add a blocking method in Feed to block control until feed is up to date ?
 */

gt::TorrentGroup::TorrentGroup()
{
}

void gt::TorrentGroup::add(std::shared_ptr<gt::Torrent> t)
{
        m_torrents_all.push_back(t);
        std::cout << m_torrents_all.size();
}

// This function is pretty shitty. There has to be a better way in sepples to
// delete elements of a vector.  I'm also of the opinion that a vector is the
// wrong container to use, because this function's complexity is linear on the
// number of elements. Imagine dealing with thousands of torrents.
void gt::TorrentGroup::remove(std::shared_ptr<gt::Torrent> t)
{
        for (size_t i = 0; i < m_torrents_all.size(); i++)
                if (m_torrents_all[i] == t)
                        m_torrents_all.erase(m_torrents_all.begin() + i);
}

int gt::TorrentGroup::update()
{
        // TODO Think of anything TorrentGroup MIGHT have to do that Core::update() doesn't
        updateStatus();
        return 0;
}

int gt::TorrentGroup::updateStatus()
{
	m_torrents_downloading .clear();
	m_torrents_seeding     .clear();
	m_torrents_checking    .clear();
	m_torrents_stopped     .clear();
	m_torrents_finished    .clear();

        for(auto i : m_torrents_all)
	{
		if(i->isPaused())
		{
			m_torrents_paused.push_back(i);
			continue;
		}

		switch(i->status().state)
		{
		case libtorrent::torrent_status::state_t::downloading:
			m_torrents_downloading.push_back(i);
			continue;
		case libtorrent::torrent_status::state_t::seeding:
			m_torrents_seeding.push_back(i);
			continue;
		case libtorrent::torrent_status::state_t::checking_files:
		case libtorrent::torrent_status::state_t::checking_resume_data:
			m_torrents_checking.push_back(i);
			continue;
		case libtorrent::torrent_status::state_t::finished:
			m_torrents_finished.push_back(i);
			continue;
		default:
			m_torrents_stopped.push_back(i);
		}
	}

	return 1;
}

gt::Core::Core(int argc, char **argv) :
	m_session(libtorrent::fingerprint("GT", 0, 0, 2, 0), 3, 0x7FFFFFFF),
	m_running(true)
{
	// TODO Resolving this without exiting like that. It should be done gracefully, call the appropriate exit/cleanup functions. i.e gt::Core::shutedown()
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

	m_torrents.update();
}

// TODO Deprecated. Migrate all uses of this function to Core::getAllTorrents()
std::vector<std::shared_ptr<gt::Torrent>> &gt::Core::getTorrents()
{
	return m_torrents.m_torrents_all;
}

gt::TorrentGroup* gt::Core::getAllTorrents()
{
	return &m_torrents;
}

// TODO Move to Utilities (if it exists) or Platform
bool gt::Core::isLink(std::string const& url)
{
	const std::string magprefix = "magnet:", httpprefix = "http:";
	return url.compare(0, magprefix.length(), magprefix) == 0 || url.compare(0, httpprefix.length(), httpprefix) == 0;
}

// Question: Should torrentAdd be responsible for calling
// TorrentGroup::updateStatus?  I believe it should be the responsibility of
// the caller, or to just leave it to whatever function is involved in calling
// update() in regular intervals.
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

		m_torrents.add(t);

		if(t->status().has_metadata && gt::Settings::settings["DefaultSequentialDownloading"] == "Yes")
			if(t->filenames().size() == 1)
			{
				std::string ext = t->filenames()[0].substr(t->filenames()[0].find_last_of('.') + 1);
				t->set_sequential_download(gt::Settings::settings["SequentialDownloadExtensions"].find(ext) != std::string::npos);
			}

		return t;
	}
}

void gt::Core::removeTorrent(std::shared_ptr<Torrent> t)
{
	//TODO : add removal of files on request
	//TODO : Remove fast resume data associated to file
	m_session.remove_torrent(libtorrent::torrent_handle(*t)); //explicit cast is required to make a copy of the underlying object
	m_torrents.remove(t);
	m_torrents.update();
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

	for(auto f : m_feedhandles)
		if(f->owners.empty())
			m_session.remove_feed(*f);

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

	for(auto tor : m_torrents.m_torrents_all)
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

	std::string fbuf;
	for(auto feed : m_feeds)
		fbuf += (std::string)*feed;

	std::ofstream feedfile(folder + "feeds.gts");
	feedfile << fbuf;
	feedfile.close();
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
		!gt::Platform::checkDirExist(folder +  "list.gts") ||
		!gt::Platform::checkDirExist(folder + "feeds.gts"))
	{
		// Also creates an empty session.
		gt::Log::Debug(std::string("Creating new session folder in: " + gt::Platform::getDefaultConfigPath()).c_str());
		std::ofstream state(folder + "state.gts");
		std::ofstream  list(folder +  "list.gts");
		std::ofstream feeds(folder + "feeds.gts");

		state.close();
		list.close();
		feeds.close();
	}

	std::fstream state;
	std::fstream list;
	std::fstream feeds;

	state.open(folder + "state.gts");
	list .open(folder + "list.gts");
	feeds.open(folder + "feeds.gts");

	if(!state.is_open())
		throw "Couldn't open state.gts";

	if(!list.is_open())
		throw "Couldn't open list.gts";

	if(!feeds.is_open())
		throw "Couldn't open feeds.gts";

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

	// describes the layout of info save in the file
	struct feedinfo
	{
		std::string url;
		std::map<std::string, std::string> filters;
		std::set<std::string> functions;
	} fi;

	// TODO: Fix this

	std::string sData; 
	while(std::getline(feeds, tmp))
		sData += tmp;

	m_feeds = gt::FeedGroup::fromString(sData, this);

	for(auto f : m_feeds)
	{
		f->onStateChanged = [f](int state, std::shared_ptr<gt::Feed> feed)
			{
				switch(state)
				{
				case 0:  if(f->onUpdateStarted) f->onUpdateStarted (feed); break;
				case 1:  if(f->onUpdateFinished)f->onUpdateFinished(feed); break;
				default: if(f->onUpdateErrored) f->onUpdateErrored (feed);
				}
			};

		f->onNewItemAvailable = [f](const libtorrent::feed_item &item, std::shared_ptr<gt::Feed> feed)
			{
				if(!f) return;
				auto items = f->getFilteredItems();
				if(std::find_if(items.begin(), items.end(), [item](libtorrent::feed_item &arg){ if(item.title == arg.title) {std::cout << item.title << " Matched with " << arg.title << std::endl; return true;} return false; }) != items.end())
					f->addItem(item);
			};
	}

	m_session.resume();
	std::deque<libtorrent::alert*> alerts;
	m_session.set_alert_mask(0x7FFFFFFF);
	m_session.pop_alerts(&alerts); // empty possible alerts here
	return 0;
}

/**
 * "update()" is a misnomer, a large amount of this function really handles
 * alerts. If update() has to do multiple things, make separate functions that
 * are called together by update()
 * TODO gt::Core::update is too large and should broken down
 */
std::shared_ptr<gt::Torrent> gt::Core::update()
{
	// * Why is this here *
	std::string sharedData = gt::Platform::readSharedData();
	if(!sharedData.empty()) gt::Log::Debug(sharedData.c_str());

	// Handle Feed stuff?
	if(sharedData.find("update_feeds") != std::string::npos)
		for(auto feeds : m_feeds)
			for(auto f : feeds->m_feeds)
				gt::Log::Debug(std::to_string(f->get_feed_status().next_update) + " remaining before update");

	// * Handle alerts *
	// Pop all pending alerts available
	std::deque<libtorrent::alert*> alerts;
	m_session.pop_alerts(&alerts);

	while(!alerts.empty())
	{
		libtorrent::alert *al = alerts.front();
		alerts.pop_front();
		switch (al->type()) {
			// Torrent state change alerts
			case libtorrent::state_changed_alert::alert_type: {
				auto scal = (libtorrent::state_changed_alert *) (al);
				gt::Torrent *tor = (gt::Torrent *) &scal->handle;
				if (tor->onStateChanged)
					tor->onStateChanged(scal->prev_state, tor);
				// Don't think what this part does is right.
				if (gt::Settings::settings["DefaultSequentialDownloading"] == "Yes" && tor->status().has_metadata) {
					if (tor->filenames().size() == 1) {
						std::string ext = tor->filenames()[0].substr(
							tor->filenames()[0].find_last_of('.') + 1);
						tor->set_sequential_download(
							gt::Settings::settings["SequentialDownloadExtensions"].find(
								ext) != std::string::npos);
					}
				}
				break;
			}
			// DHT Alerts
			case libtorrent::dht_announce_alert::alert_type:
			case libtorrent::dht_bootstrap_alert::alert_type:
			case libtorrent::dht_error_alert::alert_type:
			case libtorrent::dht_get_peers_alert::alert_type:
			case libtorrent::dht_immutable_item_alert::alert_type:
			case libtorrent::dht_mutable_item_alert::alert_type:
			case libtorrent::dht_put_alert::alert_type:
			case libtorrent::dht_reply_alert::alert_type: {
				if (m_session.is_dht_running() && gt::Settings::settings["DHTEnabled"] == "Yes") {
					// This wasn't written at time of refactoring. I'm just leaving it in
					// case someone knows what to do with dht alerts.
				}
				break;
			}
			// RSS Alerts
			case libtorrent::rss_alert::alert_type: {
				libtorrent::rss_alert *rssal = static_cast<libtorrent::rss_alert *>(al);
				gt::Feed *feed = (gt::Feed *) &rssal->handle;
				if (feed->onStateChanged)
					feed->onStateChanged(rssal->state, feed);
				break;
			}
			case libtorrent::rss_item_alert::alert_type: {
				libtorrent::rss_item_alert *rssal = static_cast<libtorrent::rss_item_alert *>(al);
				gt::Feed *feed = (gt::Feed *) &rssal->handle;
				if (feed->onItemAlert)
					feed->onItemAlert(rssal);
				break;
			}
			default:
				gt::Log::Debug(al->message());
		}
	}

	// what the fuck?
	return addTorrent(sharedData);
}

/**
 * Returns a vector of torrents that has had a state changed.
 * I don't think this is actually used anymore, in favour of gt::Torrent.onStateChanged etc.
 */
std::vector<std::shared_ptr<gt::Torrent>> gt::Core::getPendingTorrents() {
	return m_pending_torrents;
}
// TODO: Catch some signals to make sure this function is called
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

std::shared_ptr<gt::Feed> gt::Core::addFeed(std::string url)
{
	for(auto feeds : m_feeds)
		for(auto f : feeds->m_feeds)
			if(f->get_feed_status().url == url)
				return f;

	libtorrent::feed_settings fs;
	fs.url = url;
	fs.auto_download = false;
	fs.auto_map_handles = true;
	fs.default_ttl = 1;

	auto f = std::make_shared<gt::Feed>(m_session.add_feed(fs));
	m_feedhandles.push_back(f);
	return f;
}

void gt::Core::removeFeed(std::shared_ptr<gt::Feed> feed)
{
	//remove feed from the feedgroup
	//this method will delete a feedgroup in the future
/*	unsigned i = 0;
	for(i = 0; i < m_feeds.size(); ++i)
		if(feed == m_feeds[i]) m_session.remove_feed(*feed);
		m_feeds.erase(m_feeds.begin() + i);*/
}

std::shared_ptr<gt::FeedGroup> gt::Core::addFeedGroup(std::string name)
{
	for(auto fg : m_feeds)
		if(fg->name == name)
			return fg;
	auto fg = std::make_shared<gt::FeedGroup>();
	fg->name = name;
	return fg;
}
