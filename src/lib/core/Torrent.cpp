#include "Torrent.hpp"

Torrent::Torrent(std::string path)
{
	m_torrent_params.save_path = "./";
	m_torrent_params.ti = new libtorrent::torrent_info(path);
}

libtorrent::add_torrent_params Torrent::getTorrentParams()
{
	return m_torrent_params;
}