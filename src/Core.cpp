#include "Core.hpp"
#include "Log.hpp"
#include "libtorrent/alert.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/create_torrent.hpp"

gt::Core::Core() :
    m_running(true) {
    // Fuck your deprecated shit, we're going void down in here
    // tl;dr, figure out something useful to use the error code for,
    // like handling what the fuck might happen if listen_on fails kthnx
	if(loadSession(".config"))
		gt::Log::Debug("Didn't load state (file not found or not enough permissions)... Skipping...");

    libtorrent::error_code ec;
    m_session.listen_on(make_pair(6881, 6889), ec);
    if (ec.value() != 0)
        gt::Log::Debug(ec.message().c_str());
}

bool gt::Core::isMagnetLink(string const& url) {
    const string prefix = "magnet:";
    return url.compare(0, prefix.length(), prefix) == 0;
}

string gt::Core::getDefaultSavePath() {
#ifndef _WIN32
    char *savepath = getenv("HOME");
    return savepath == NULL ? string("") : string(savepath) + "/Downloads";
#else
    char *savedrive = getenv("HOMEDRIVE");
    char *savepath = getenv("HOMEPATH");
    return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
#endif
}

shared_ptr<gt::Torrent> gt::Core::addTorrent(string path, vector<char> *resumedata) {
    if (path.empty())
        return shared_ptr<gt::Torrent>();//Use default constructor instead of null

    shared_ptr<gt::Torrent> t;

    try {
        t = make_shared<gt::Torrent>(path);
        //pointer necessary to catch exception as a shared ptr would go out of scope
    } catch (int exception) {
        return shared_ptr<gt::Torrent>();
        //Return null if invalid torrent to be handled by GtkMainWindow
    }

    libtorrent::error_code ec;
	auto params = t->getTorrentParams();
	params.resume_data = resumedata; //TODO: Look if fast resume data exists for this torrent
    libtorrent::torrent_handle h = m_session.add_torrent(params, ec);

    if (ec.value() != 0) {
        gt::Log::Debug(ec.message().c_str());
        return shared_ptr<gt::Torrent>();
    } else {
        t->setHandle(h);
        m_torrents.push_back(t);
        return t;
    }
}

void gt::Core::removeTorrent(shared_ptr<Torrent> t) {
    //TODO : add removal of files on request
	//Todo : Remove fast resume data associated to file
    m_session.remove_torrent(t->getHandle());
}

/*
 * Where path is relative to the executable
 */
void gt::Core::saveSession(string folder) {
	libtorrent::entry ent;
	m_session.pause();
	m_session.save_state(ent);
	

	//Todo: make a plateform independant version of the following
	struct stat st;
	if(stat(folder.c_str(), &st)) //stat() returns 0 if the folder exist
		mkdir(folder.c_str(), 0755); 

	if(stat(string(folder + "/meta").c_str(), &st))
		mkdir(string(folder + "/meta").c_str(), 0755); 

	ofstream file(folder + "/state.gts");
	ofstream list(folder + "/list.gts");
	
	if(!file)
		throw "Couldn't open the file.";
	
	if(!list)
		throw "Couldn't open the file.";
	
	bencode(ostream_iterator<char>(file), ent);
	file.close();
						
	int count = 0;

	for(auto tor : m_torrents) {
		if(!tor->getHandle().is_valid()) continue;
		if(!tor->getHandle().status().has_metadata) continue;
		if(!tor->getHandle().need_save_resume_data()) continue;

		auto ent = libtorrent::create_torrent(tor->getHandle().get_torrent_info()).generate();
        ofstream out((folder + "/meta/" + tor->getHandle().get_torrent_info().name() + ".torrent").c_str(), std::ios_base::binary);
        out.unsetf(ios_base::skipws);
        bencode(ostream_iterator<char>(out), ent);
		
		tor->getHandle().save_resume_data();
		++count;
	}

	while(count) {
		libtorrent::alert const *al = m_session.wait_for_alert(libtorrent::seconds(10));
		unique_ptr<libtorrent::alert> holder = m_session.pop_alert();
		gt::Log::Debug("Caught alert...");

		switch (al->type()) {
		case libtorrent::save_resume_data_alert::alert_type: break;
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
        ofstream out((folder + "/meta/" + h.get_torrent_info().name() + ".fastresume").c_str(), std::ios_base::binary);
        out.unsetf(ios_base::skipws);
		list << h.get_torrent_info().name() << '\n';
        bencode(ostream_iterator<char>(out), *rd->resume_data);
        --count;
	}

	list.close();
}

int gt::Core::loadSession(string folder) {
	libtorrent::lazy_entry ent;
	libtorrent::error_code ec;

	ifstream file(folder + "/state.gts");
	ifstream list(folder + "/list.gts");

	if(!file || !list)
		return -1;
	
	string benfile, tmp;

	do {
		getline(file, tmp);
		benfile += tmp;
	} while(file);

	lazy_bdecode(benfile.c_str(), benfile.c_str() + benfile.size(), ent, ec);
	m_session.load_state(ent);

	while(getline(list, tmp))
	{
		libtorrent::add_torrent_params params;
		vector<char> resumebuff;
		ifstream resumedata(folder + "/meta/" + tmp + ".fastresume");
		while(resumedata)
			resumebuff.push_back(resumedata.get());
		auto t = addTorrent(folder + "/meta/" + tmp + ".torrent", &resumebuff);
	}

	m_session.resume();
	return 0;
}

void gt::Core::update() {
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

void gt::Core::shutdown() {
    gt::Log::Debug("Shutting down core library...");
	saveSession(".config");
    m_running = false;
}
