#include <core/Core.hpp>
#include "Torrent.hpp"

#define T_PPM 1000000.f

Torrent::Torrent(string path) :
	m_path(path)
{
	m_torrent_params.save_path = "./";

	if (Core::isMagnetLink(path))
		m_torrent_params.url = path;
	else
		m_torrent_params.ti = boost::shared_ptr<libtorrent::torrent_info>(new libtorrent::torrent_info(path)); //Propery create shared_ptr with constructor instead of = operator.
}

bool Torrent::pollEvent(gt::Event &event)
{
	if (getTotalProgress() >= 100) {
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

string Torrent::getPath()
{
	return m_path;
}

float Torrent::getTotalProgress()
{
	libtorrent::torrent_status s = m_handle.status();

	return ((float) s.progress_ppm / (float) T_PPM) * 100;
}

unsigned int Torrent::getDownloadRate()
{
	return m_handle.status().download_rate;
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

unsigned int Torrent::getTotalLeechers()
{
	return m_handle.status().num_incomplete;
}

libtorrent::torrent_status::state_t Torrent::getState()
{
	return m_handle.status().state;
}

string Torrent::getTextState()
{
	switch (getState()) {
		case libtorrent::torrent_status::checking_files:
			return "Checking";
		break;
		case libtorrent::torrent_status::seeding:
			return "Seeding";
		break;
		case libtorrent::torrent_status::downloading:
		default:
			char p[5];
			sprintf(p, "%.1f %%", getTotalProgress());
			return p;
		break;
	}
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}
