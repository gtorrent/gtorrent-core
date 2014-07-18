#include "Core.hpp"
#include "Log.hpp"

gt::Core::Core()
{
	// Fuck your deprecated shit, we're going void down in here
	// tl;dr, figure out something useful to use the error code for,
	// like handling what the fuck might happen if listen_on fails kthnx
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

string gt::Core::getDefaultSavePath()
{
	// Items like this will _probably_ be a virtual function for each platform
	// Or handled somewhere else in the core, like Plat class.
#ifndef _WIN32
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath) + "/Downloads";
#else
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive) + string(savepath) + "/Downloads";
#endif
}

shared_ptr<Torrent> gt::Core::addTorrent(string path)
{
	if (path.empty())
		return shared_ptr<Torrent>();//Use default constructor instead of null
	shared_ptr<Torrent> t;
	try
	{
		t = make_shared<Torrent>(path);
		//pointer necessary to catch exception as a shared ptr would go out of scope
	}
	catch (int exception)
	{
		return shared_ptr<Torrent>();
		//Return null if invalid torrent to be handled by GtkMainWindow
	}
	libtorrent::error_code ec;
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams(), ec);
	if (ec.value() != 0)
	{
		gt::Log::Debug(ec.message().c_str());
		return shared_ptr<Torrent>();
	}
	else
	{
		t->setHandle(h);
		m_torrents.push_back(t);
		return t;
	}

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
