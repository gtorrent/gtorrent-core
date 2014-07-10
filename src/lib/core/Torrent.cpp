#include "Torrent.hpp"

Torrent::Torrent(std::string path)
{
	m_torrent_params.save_path = "./";
	m_torrent_params.ti = new libtorrent::torrent_info(path);
}

bool Torrent::pollEvent()
{
	libtorrent::torrent_status s = m_handle.status();

	if (s.progress_ppm >= 1000000)
	{
		printf("Finished downloading torrent.\n");
		return true;
	}

	return false;
}

libtorrent::add_torrent_params Torrent::getTorrentParams()
{
	return m_torrent_params;
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}