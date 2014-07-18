#include "Core.hpp"

gt::Core::Core()
{
	libtorrent::error_code ec;
	m_session.listen_on(make_pair(6881, 6889), ec);
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
	return savepath == NULL ? string("") : string(savepath)+"/Downloads";
	#else
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive)+string(savepath)+"/Downloads";
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
	
	shared_ptr<Torrent> t = make_shared<Torrent>(path);
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams());

	t->setHandle(h);
	m_torrents.push_back(t);

	return t;
}