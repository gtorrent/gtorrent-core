#include "Torrent.hpp"

#define T_PPM 1000000

Torrent::Torrent(std::string path) :
	m_path(path)
{
	m_torrent_params.save_path = "./";
	m_torrent_params.ti = new libtorrent::torrent_info(path);
}

bool Torrent::pollEvent(gt::Event &event)
{
	if (getTotalProgress() >= 100)
	{
		event.type = gt::Event::DownloadCompleted;
		return true;
	}

	return false;
}

libtorrent::add_torrent_params Torrent::getTorrentParams()
{
	return m_torrent_params;
}

std::string Torrent::getPath()
{
	return m_path;
}

unsigned int Torrent::getTotalProgress()
{
	libtorrent::torrent_status s = m_handle.status();

	return (s.progress_ppm / T_PPM) * 100;
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}