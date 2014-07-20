#include "Core.hpp"
#include "Log.hpp"
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

gt::Core::Core() :
	m_running(true)
{
	// Fuck your deprecated shit, we're going void down in here
	// tl;dr, figure out something useful to use the error code for,
	// like handling what the fuck might happen if listen_on fails kthnx
	libtorrent::error_code ec;
	m_session.listen_on(make_pair(6881, 6889), ec);
    torrentCopyPath = getDefaultTorrentCopyPath();
    // Make directories for copying files 
#ifdef _WIN32
    _mkdir(torrentCopyPath.substr(0,torrentCopyPath.find_last_of("\\/")).c_str());
    _mkdir(torrentCopyPath.c_str());
#else
    mkdir(torrentCopyPath.substr(0,torrentCopyPath.find_last_of("\\/")).c_str(), 0744);
    mkdir(torrentCopyPath.c_str(), 0744);
#endif
}

bool gt::Core::isMagnetLink(string const& url)
{
	const string prefix = "magnet:";
	return url.compare(0, prefix.length(), prefix) == 0;
}

bool gt::Core::isRunning()
{
	return m_running;
}

string gt::Core::getDefaultSavePath()
{
#ifndef _WIN32
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath) + "/Downloads";
#else
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
#endif
}

vector<shared_ptr<Torrent> > &gt::Core::getTorrents()
{
	return m_torrents;
}

shared_ptr<Torrent> gt::Core::addTorrent(string path)
{
	if (path.empty())
		return NULL;

    // Copy torrent file to torrentcopypath
    ifstream src(path, ios::binary);
    // Set the output filename
    string outpath = path;
    int pos;
    pos = path.find_last_of("\\/");
    outpath = torrentCopyPath + "/" + outpath.substr(pos + 1, outpath.size() - pos - 1);
    ofstream dst(outpath, ios::binary);
    // Copy data
    dst << src.rdbuf();


	shared_ptr<Torrent> t = make_shared<Torrent>(path);
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams());

	t->setHandle(h);
	m_torrents.push_back(t);

	// TODO: Event polling for magnet update information
	//libtorrent::file_storage fs = info.files();
	//libtorrent::torrent_info info = h.get_torrent_info();

	// An Example For Fetching All Files From Torrent:

	/*unsigned int numFiles = fs.num_files();

	printf("Total Files: %i\n", fs.num_files());
	printf("=========================================\n");

	for (unsigned int i = 0; i < numFiles; ++i)
	{
		libtorrent::file_entry e = fs.at(i);

		printf("Name:\t\t%s\n", e.path.c_str());
		printf("Size:\t\t%i\n", e.size);
		printf("-----------------------------------------\n");
	}

	printf("Total Size:\t%Li\n\n", fs.total_size());

	printf("Downloading data from \"%s\"...\n", path.c_str());*/

	return t;
}

string gt::Core::getDefaultTorrentCopyPath()
{
#ifndef _WIN32
    char* copyPath = getenv("HOME");
    return copyPath = NULL ? string("") : string(copyPath) + "/.config/gtorrent/torrents";
#else
	char *copyPath = getenv("LOCALAPPDATA");
	return copyPath == NULL ? string("") : string(copyPath) + "/gtorrent/torrents";
#endif
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
	m_running = false;
}
