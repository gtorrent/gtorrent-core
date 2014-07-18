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

unsigned int Torrent::getUploadRate()
{
        return m_handle.status().upload_rate;
}

string Torrent::getTextUploadRate()
{
        std::ostringstream upr;

        long double uprate = getUploadRate() / 1024;

        if (uprate <= 0) {
                upr << string();
        } else if (uprate> 0 && uprate < 1024) {
                upr << uprate << " KB/s";
        } else if (uprate >= 1024 && uprate < (1024*1024)) {
                upr << setprecision(2) << (uprate / 1024) << " MB/s";
        } else if (uprate >= (1024*1024) && uprate<(1024*1024*1024)) {
                upr << setprecision(2) << (uprate / 1024 / 1024) << " GB/s";
        } else if (uprate >= (1024*1024*1024)) {
                upr << setprecision(2) << (uprate / 1024 / 1024 / 1024) << " sanic/s";
        }
        return upr.str();
}

unsigned int Torrent::getDownloadRate()
{
        return m_handle.status().download_rate;
}

string Torrent::getTextDownloadRate()
{
	std::ostringstream dnr;

	long double downrate = getDownloadRate() / 1024;

	if (downrate <= 0) {
		dnr << string();
	} else if (downrate> 0 && downrate < 1024) {
		 dnr << downrate << " KB/s";
        } else if (downrate >= 1024 && downrate < (1024*1024)) {
		dnr << setprecision(2) << (downrate / 1024) << " MB/s";
	} else if (downrate >= (1024*1024) && downrate<(1024*1024)) {
		dnr << setprecision(2) << (downrate / 1024 / 1024) << " GB/s";
	} else if (downrate >= (1024*1024*1024)) {
		dnr << setprecision(2) << (downrate / 1024 / 1024 / 1024) << " sanic/s";
	}
	return dnr.str();
}

unsigned int Torrent::getTotalUploaded()
{
        return m_handle.status().total_upload;
}

string Torrent::getTextTotalUploaded()
{
        std::ostringstream ttu;

        double uploaded = getTotalUploaded() / 1024;

        if (uploaded <= 0) {
                ttu << string();
	} else if (uploaded > 0 && uploaded <= 1024) {
                ttu << setprecision(2) << uploaded << " KB";
        }  else if (uploaded > 1024 && uploaded <= (1024*1024)) {
                ttu << setprecision(2) << (uploaded / 1024) << " MB";
        } else if (uploaded > (1024*1024) && uploaded <= (1024*1024*1024)) {
                ttu << (uploaded / 1024 / 1024) << " GB";
        } else if (uploaded > (1024*1024*1024)) {
                ttu << (uploaded / 1024 / 1024 / 1024) << " TB";
        }


        return ttu.str();
}


unsigned int Torrent::getTotalDownloaded()
{
        return m_handle.status().total_download;
}

string Torrent::getTextTotalDownloaded()
{
        std::ostringstream ttd;

        double downloaded = getTotalDownloaded() / 1024;

        if (downloaded<=0) {
                ttd << string();
        } else if (downloaded > 0 && downloaded <= 1024) {
                ttd << setprecision(2) << downloaded << " KB";
        }  else if (downloaded > 1024 && downloaded <= (1024*1024)) {
                ttd << setprecision(2) << (downloaded / 1024) << " MB";
        } else if (downloaded > (1024*1024) && downloaded <= (1024*1024*1024)) {
                ttd << (downloaded / 1024 / 1024) << " GB";
        } else if (downloaded > (1024*1024*1024)) {
                ttd << (downloaded / 1024 / 1024 / 1024) << " TB";
        }

        return ttd.str();
}

float Torrent::getTotalRatio()
{
	if (getTotalDownloaded()==0) {
		return 0.0f;
	} else {
		float totalRatio = float(getTotalUploaded()) / float(getTotalDownloaded());
		return totalRatio;
	}
}

string Torrent::getTextTotalRatio()
{
	std::stringstream ttr (stringstream::in | stringstream::out);

	float ratio = getTotalRatio();

	ttr << setprecision(2) << ratio;

	return ttr.str();
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}
