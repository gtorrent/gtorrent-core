#include <core/Core.hpp>
#include "Torrent.hpp"
#define T_PPM 1000000.f

Torrent::Torrent(string path) :
	m_path(path)
{
	setSavePath(""); //TODO add argument to allow user to override the default save path of $HOME/Downloads
	if (gt::Core::isMagnetLink(path))
		m_torrent_params.url = path;
	else
			//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
			//Using decltype allows us to make it compatible with both versions while still properly using the constructor to avoid a compiler error on boost 1.55 when the = operator is used with a pointer.
			m_torrent_params.ti = decltype(m_torrent_params.ti)(new libtorrent::torrent_info(path)); 

}

void Torrent::setSavePath(string savepath)
{
	if (savepath.empty())
		savepath = gt::Core::getDefaultSavePath();
	if (savepath.empty())
		savepath="./"; //Fall back to ./ if $HOME is not set
	m_torrent_params.save_path = savepath; 
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
	return m_handle.status().num_peers - m_handle.status().num_seeds;
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
			std::ostringstream o;
			o << setprecision(2) << getTotalProgress() << " %";
			return o.str();
		break;
	}
}

string Torrent::getTextDownloadRate()
{
	std::ostringstream oss;

	double rate = getDownloadRate() / 1024;

	if (rate <= 0) {
		oss << string();
	} else if (rate > 1024) {
		oss << setprecision(2) << (rate / 1024) << " MB/s";
	} else {
		oss << rate << " KB/s";
	}

	return oss.str();
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}
