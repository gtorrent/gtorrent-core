#include "TorrentEngine.hpp"

TorrentEngine::TorrentEngine()
{
	m_session.listen_on(std::make_pair(6881, 6889));
}

void TorrentEngine::addTorrent(std::string path)
{
	printf("Downloading data from \"%s\"\n", path.c_str());

	libtorrent::add_torrent_params p;
	p.save_path = "./";
	p.ti = new libtorrent::torrent_info(path);

	libtorrent::torrent_handle h = m_session.add_torrent(p);
}