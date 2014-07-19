#include <core/Core.hpp>
#include "Torrent.hpp"
#include "Log.hpp"
#define T_PPM 1000000.f

Torrent::Torrent(string path) :
	m_path(path)
{
	setSavePath(""); //TODO add argument to allow user to override the default save path of $HOME/Downloads
	if (gt::Core::isMagnetLink(path))
		m_torrent_params.url = path;
	else
	{
		//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
		//Using decltype allows us to make it compatible with both versions while still properly using the constructor to avoid a compiler error on boost 1.55 when the = operator is used with a pointer.
		libtorrent::error_code ec;
		decltype(m_torrent_params.ti) tester = decltype(m_torrent_params.ti)(new libtorrent::torrent_info(path, ec));
		if (ec.value() == 0)
		{
			ifstream torrentcheck(path);
			bool isempty = torrentcheck.peek() == ifstream::traits_type::eof();
			torrentcheck.close();
			if (isempty)
			{
				gt::Log::Debug("The torrent file was empty");
				throw - 1;
			}
			m_torrent_params.ti = tester;//If no exception was thrown add the torrent
		}
		else
		{
			gt::Log::Debug(ec.message().c_str());//Call deconstructor?
			throw - 1; //Throw error if construction of libtorrent::torrent_info fails.
		}
	}
}

void Torrent::setSavePath(string savepath)
{
	if (savepath.empty())
		savepath = gt::Core::getDefaultSavePath();
	if (savepath.empty())
		savepath = "./"; //Fall back to ./ if $HOME is not set
	m_torrent_params.save_path = savepath;
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

string Torrent::getPath()
{
	return m_path;
}

boost::int64_t Torrent::getActive()
{
	return m_handle.status().active_time;
}

string Torrent::getTextActive()
{
	ostringstream tas;

	boost::int64_t  active = getActive();
	//TODO: Figure out leap years
	if (active <= 0)
	{
		tas << string();
	}
	else if (active > 0 && active <= 60)    //seconds
	{
		tas << fixed << setprecision(2) << active;
	}
	else if (active > 60 && active <= (60 * 60))  //minutes
	{
		tas << ((active - active % 60) / 60) << ":" << (active % 60);
	}
	else if (active > (60 * 60) && active <= (60 * 60 * 24)) //hours
	{
		tas << ((active - active % 60) / 60 / 24) << ":" << ((active - active % 60) / 60) << ":" << (active % 60);
	}
	else if (active > (60 * 60 * 24) && active <= (60 * 60 * 24 * 7)) //days
	{
		tas << ((active - active % 60) / 60 / 24) << ":" << ((active - active % 60) / 60) << ":" << ((active - active % 60) / 60) << ":" << (active % 60);
	}
	else if (active > (60 * 60 * 24 * 7) && active <= (60 * 60 * 24 * 365)) //weeks
	{
		tas << ((active - active % 60) / 60 / 24 / 7) << ":" << ((active - active % 60) / 60 / 24) << ":" << ((active - active % 60) / 60) << ":" << ((active - active % 60) / 60) << ":" << (active % 60);
	}
	else if (active > (60 * 60 * 24 * 365)) //years
	{
		tas << ((active - active % 60) / 60 / 24 / 365) << ":" << ((active - active % 60) / 60 / 24 / 7) << ":" << ((active - active % 60) / 60 / 24) << ":" << ((active - active % 60) / 60) << ":" << ((active - active % 60) / 60) << ":" << (active % 60);
	}
	return tas.str();
}

boost::int64_t Torrent::getWanted()
{
        return m_handle.status().total_wanted;
}

boost::int64_t Torrent::getEta()
{
	if (getDownloadRate() <= 0){
		return 817;
	}else{
	return (getWanted() / getDownloadRate());
	}
}

string Torrent::getTextEta()
{
	std::ostringstream tet;

	boost::int64_t  eta = getEta();
	//TODO: Figure out leap years
	if (eta <= 0)
	{
		tet << string();
	}
	else if (eta > 0 && eta <= 60)    //seconds
	{
		tet << fixed << setprecision(2) << eta;
	}
	else if (eta > 60 && eta <= (60 * 60))  //minutes
	{
		tet << ((eta - eta % 60) / 60) << ":" << (eta % 60);
	}
	else if (eta > (60 * 60) && eta <= (60 * 60 * 24)) //hours
	{
		tet << ((eta - eta % 60) / 60 / 24) << ":" << ((eta - eta % 60) / 60) << ":" << (eta % 60);
	}
	else if (eta > (60 * 60 * 24) && eta <= (60 * 60 * 24 * 7)) //days
	{
		tet << ((eta - eta % 60) / 60 / 24) << ":" << ((eta - eta % 60) / 60) << ":" << ((eta - eta % 60) / 60) << ":" << (eta % 60);
	}
	else if (eta > (60 * 60 * 24 * 7) && eta <= (60 * 60 * 24 * 365)) //weeks
	{
		tet << ((eta - eta % 60) / 60 / 24 / 7) << ":" << ((eta - eta % 60) / 60 / 24) << ":" << ((eta - eta % 60) / 60) << ":" << ((eta - eta % 60) / 60) << ":" << (eta % 60);
	}
	else if (eta > (60 * 60 * 24 * 365)) //years
	{
		tet << ((eta - eta % 60) / 60 / 24 / 365) << ":" << ((eta - eta % 60) / 60 / 24 / 7) << ":" << ((eta - eta % 60) / 60 / 24) << ":" << ((eta - eta % 60) / 60) << ":" << ((eta - eta % 60) / 60) << ":" << (eta % 60);
	}
	return tet.str();
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
	switch (getState())
	{
	case libtorrent::torrent_status::checking_files:
		return "Checking";
		break;
	case libtorrent::torrent_status::seeding:
		return "Seeding";
		break;
	case libtorrent::torrent_status::downloading:
	default:
		ostringstream o;
		int precision = 1;
		if (m_torrent_params.ti != NULL) //m_torrent_params.ti is not initial initialized for magnet links
			if (m_torrent_params.ti->total_size() < 1024 * 1024 * 1024)
				precision = 0;//Set 0 decimal places if file is less than 1 gig.
		o << fixed << setprecision(precision) << getTotalProgress() << " %";
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
	ostringstream upr;

	long double uprate = getUploadRate() / 1024;

	if (uprate <= 0)
	{
		upr << string();
	}
	else if (uprate > 0 && uprate < 1024)
	{
		upr << uprate << " KB/s";
	}
	else if (uprate >= 1024 && uprate < (1024 * 1024))
	{
		upr << fixed << setprecision(3) << (uprate / 1024) << " MB/s";
	}
	else if (uprate >= (1024 * 1024) && uprate < (1024 * 1024 * 1024))
	{
		upr <<  fixed << setprecision(3) << (uprate / 1024 / 1024) << " GB/s";
	}
	else if (uprate >= (1024 * 1024 * 1024))
	{
		upr <<  fixed << setprecision(3) << (uprate / 1024 / 1024 / 1024) << " sanic/s";
	}
	return upr.str();
}

unsigned int Torrent::getDownloadRate()
{
	return m_handle.status().download_rate;
}

string Torrent::getTextDownloadRate()
{
	ostringstream dnr;

	long double downrate = getDownloadRate() / 1024;

	if (downrate <= 0)
	{
		dnr << string();
	}
	else if (downrate > 0 && downrate < 1024)
	{
		dnr << downrate << " KB/s";
	}
	else if (downrate >= 1024 && downrate < (1024 * 1024))
	{
		dnr <<  fixed << setprecision(3) << (downrate / 1024) << " MB/s";
	}
	else if (downrate >= (1024 * 1024) && downrate < (1024 * 1024))
	{
		dnr <<  fixed << setprecision(3) << (downrate / 1024 / 1024) << " GB/s";
	}
	else if (downrate >= (1024 * 1024 * 1024))
	{
		dnr <<  fixed << setprecision(3) << (downrate / 1024 / 1024 / 1024) << " sanic/s";
	}

	return dnr.str();
}

boost::int64_t Torrent::getTotalUploaded()
{
	return m_handle.status().total_upload;
}

string Torrent::getTextTotalUploaded()
{
	ostringstream ttu;

	boost::int64_t uploaded = getTotalUploaded();

	if (uploaded <= 0.0f)
	{
		ttu << string();
	}
	else if (uploaded > 0.0f && uploaded <= 1024.f)
	{
		ttu << fixed << setprecision(3) << uploaded << " B";
	}
	else if (uploaded > 1024.f && uploaded <= (1024.f * 1024.f))
	{
		ttu << fixed << setprecision(3) << (uploaded / 1024.f) << " KB";
	}
	else if (uploaded > (1024.f * 1024.f) && uploaded <= (1024.f * 1024.f * 1024.f))
	{
		ttu << fixed << setprecision(3) << (uploaded / 1024.f / 1024.f) << " MB";
	}
	else if (uploaded > (1024.f * 1024.f * 1024.f))
	{
		ttu << fixed << setprecision(3) << (uploaded / 1024.f / 1024.f / 1024.f) << " GB";
	}

	return ttu.str();
}


boost::int64_t Torrent::getTotalDownloaded()
{
	return m_handle.status().total_download;
}

string Torrent::getTextTotalDownloaded()
{
	ostringstream ttd;

	boost::int64_t  downloaded = getTotalDownloaded();

	if (downloaded <= 0)
	{
		ttd << string();
	}
	else if (downloaded > 0 && downloaded <= 1024.f)
	{
		ttd << fixed << setprecision(3) << downloaded << " B";
	}
	else if (downloaded > 1024.f && downloaded <= (1024.f * 1024.f))
	{
		ttd << fixed << setprecision(3) << (downloaded / 1024.f) << " KB";
	}
	else if (downloaded > (1024.f * 1024.f) && downloaded <= (1024.f * 1024.f * 1024.f))
	{
		ttd << fixed << setprecision(3)  << (downloaded / 1024.f / 1024.f) << " MB";
	}
	else if (downloaded > (1024.f * 1024.f * 1024.f))
	{
		ttd << fixed << setprecision(3) << (downloaded / 1024.f / 1024.f / 1024.f) << " GB";
	}
	return ttd.str();
}

boost::int64_t Torrent::getSize()
{
        return m_handle.status().total_wanted;
}

string Torrent::getTextSize()
{
	std::ostringstream ts;

	boost::int64_t  size = getSize();

	if (size <= 0)
	{
		ts << string();
	}
	else if (size > 0 && size <= 1024.f)
	{
		ts << fixed << setprecision(3) << size << " B";
	}
	else if (size > 1024.f && size <= (1024.f * 1024.f))
	{
		ts << fixed << setprecision(3) << (size / 1024.f) << " KB";
	}
	else if (size > (1024.f * 1024.f) && size <= (1024.f * 1024.f * 1024.f))
	{
		ts << fixed << setprecision(3)  << (size / 1024.f / 1024.f) << " MB";
	}
	else if (size > (1024.f * 1024.f * 1024.f))
	{
		ts << fixed << setprecision(3) << (size / 1024.f / 1024.f / 1024.f) << " GB";
	}
	return ts.str();
}

boost::int64_t Torrent::getRemaining()
{
        return m_handle.status().total_wanted - m_handle.status().total_wanted_done;
}

string Torrent::getTextRemaining()
{
	std::ostringstream tr;

	boost::int64_t remaining = getRemaining();

	if (remaining <= 0)
	{
		tr << string();
	}
	else if (remaining > 0 && remaining <= 1024.f)
	{
		tr << fixed << setprecision(3) << remaining << " B";
	}
	else if (remaining > 1024.f && remaining <= (1024.f * 1024.f))
	{
		tr << fixed << setprecision(3) << (remaining / 1024.f) << " KB";
	}
	else if (remaining > (1024.f * 1024.f) && remaining <= (1024.f * 1024.f * 1024.f))
	{
		tr << fixed << setprecision(3)  << (remaining / 1024.f / 1024.f) << " MB";
	}
	else if (remaining > (1024.f * 1024.f * 1024.f))
	{
		tr << fixed << setprecision(3) << (remaining / 1024.f / 1024.f / 1024.f) << " GB";
	}
	return tr.str();
}

boost::int64_t Torrent::getTorrentSize()
{
	return m_handle.status().total_wanted;
}

boost::int64_t Torrent::timeRemaining()
{
	if(getDownloadRate() > 0)
		return getTorrentSize() / getDownloadRate();
	else
		return 0;
}

string Torrent::getTextTimeRemaining()
{
	//Very crude, pls to be fixing me, kthnx
	ostringstream oss;

	double time_s = timeRemaining();

	int hours = 0;
	int mins = time_s / 60;

	if(mins >= 60)
	{
		hours = mins / 60;
		mins = ((mins / hours) - 60) * hours;

		if(hours == 1)
			oss << hours << " Hour, " << mins << " Mins";
		else
			oss << hours << " Hours, " << mins << " Mins";
	}
	else
	{
		if(mins == 1)
			oss << mins << " Minute";
		else
			oss << mins << " Minutes";
	}
	
	return oss.str();
}

float Torrent::getTotalRatio()
{
	if (getTotalDownloaded() == 0)
	{
		return 0.0f;
	}
	else
	{
		float totalRatio = float(getTotalUploaded()) / float(getTotalDownloaded());
		return totalRatio;
	}
}

string Torrent::getTextTotalRatio()
{
	stringstream ttr (stringstream::in | stringstream::out);

	float ratio = getTotalRatio();

	ttr << fixed << setprecision(3) << ratio;

	return ttr.str();
}

void Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}
