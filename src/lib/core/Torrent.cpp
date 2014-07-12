#include "Torrent.hpp"

#define T_PPM 1000000.f

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

libtorrent::torrent_handle &Torrent::getHandle()
{
	return m_handle;
}

std::string Torrent::getPath()
{
	return m_path;
}

float Torrent::getTotalProgress()
{
	libtorrent::torrent_status s = m_handle.status();

	return ((float) s.progress_ppm / (float) T_PPM) * 100;
}

unsigned int Torrent::getPPMProgress()
{
	libtorrent::torrent_status s = m_handle.status();

	return s.progress_ppm;
}

unsigned int Torrent::getTotalSeeders()
{
	return m_handle.status().num_seeds;
}

unsigned int Torrent::getTotalPeers()
{
	return m_handle.status().num_peers;
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}