#include "Core.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"
#include "libtorrent/alert.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/create_torrent.hpp"

gt::Core::Core() :
	m_running(true)
{
	// Fuck your deprecated shit, we're going void down in here
	// tl;dr, figure out something useful to use the error code for,
	// like handling what the fuck might happen if listen_on fails kthnx
	loadSession(gt::Platform::getDefaultConfigPath());
    gt::Settings::parse("config");

	libtorrent::error_code ec;
	m_session.listen_on(make_pair(6881, 6889), ec);
	if (ec.value() != 0)
		gt::Log::Debug(ec.message().c_str());
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
	params.resume_data = resumedata; //TODO: Look if fast resume data exists for this torrent
	libtorrent::torrent_handle h = m_session.add_torrent(params, ec);

	if (ec.value() != 0)
	{
		gt::Log::Debug(ec.message().c_str());
		return shared_ptr<gt::Torrent>();
	}
	else
	{
		t->setHandle(h);
		m_torrents.push_back(t);
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
	cout << endl << i << endl << endl;
	while(i < m_torrents.size() - 1)
	{
		m_torrents[i] = m_torrents[i + 1];
		++i;
	}
	m_torrents.resize(m_torrents.size() - 1);
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

		auto ent = libtorrent::create_torrent(tor->getHandle().get_torrent_info()).generate();
		ofstream out((folder + "meta/" + tor->getHandle().get_torrent_info().name() + ".torrent").c_str(), std::ios_base::binary);
		out.unsetf(ios_base::skipws);
		bencode(ostream_iterator<char>(out), ent);

		tor->getHandle().save_resume_data();
		++count;
	}

	while(count)
	{
		libtorrent::alert const *al = m_session.wait_for_alert(libtorrent::seconds(10));
		unique_ptr<libtorrent::alert> holder = m_session.pop_alert();
		gt::Log::Debug("Caught alert...");

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
		ofstream out((folder + "meta/" + h.get_torrent_info().name() + ".fastresume").c_str(), std::ios_base::binary);
		out.unsetf(ios_base::skipws);
		list << h.get_torrent_info().name() << '\n';
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

	if (!(gt::Platform::checkDirExist(folder)               &&
		  gt::Platform::checkDirExist(folder + "state.gts") && 
		  gt::Platform::checkDirExist(folder + "list.gts")))
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

// FIXME : pls halp -- nyanpasu
// This probably isn't necessary and there's a better way to allow
// external code to get the save path used by the core.
// Also, maybe inline this.
string gt::Core::getDefaultSavePath()
{
	return gt::Platform::getDefaultSavePath();
}

void gt::Core::update()
{
	/*auto iter = begin(m_torrents);

	  while (iter != end(m_torrents))
	  {
	  auto &t = **iter;

	  gt::Event event;

	  if (t.pollEvent(event))
	  {
	  switch (event.type)
	  {
	  case gt::Event::DownloadCompleted:
	  printf("Done!\n");
	  iter = m_torrents.erase(iter);
	  break;
	  }
	  }
	  else
	  {
	  ++iter;
	  }
	  }*/
}

void gt::Core::shutdown()
{
	gt::Log::Debug("Shutting down core library...");
	saveSession(gt::Platform::getDefaultConfigPath());
	m_running = false;
}
