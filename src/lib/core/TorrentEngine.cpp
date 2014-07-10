#include "TorrentEngine.hpp"
#include "Torrent.hpp"

TorrentEngine::TorrentEngine()
{
	m_session.listen_on(std::make_pair(6881, 6889));
}

void TorrentEngine::addTorrent(std::string path)
{
	printf("Downloading data from \"%s\".\n", path.c_str());

	std::shared_ptr<Torrent> t = std::shared_ptr<Torrent>(new Torrent(path));
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams());
}