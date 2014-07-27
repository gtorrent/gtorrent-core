#include "Core.hpp"
#include "Torrent.hpp"
#include "Log.hpp"
#define T_PPM 1000000.f

// format 0d 0h 0m 0s
string getTimeString( boost::int64_t time_s )
{
	if ( time_s <= 0 )
		return "???";

	boost::int64_t time_m = time_s / 60;
	time_s %= 60;
	boost::int64_t time_h = time_m / 60;
	time_m %= 60;
	boost::int64_t time_d = time_h / 24;
	time_h %= 24;

	ostringstream time_string;

	if ( time_d > 0 )
		time_string << time_d << "d ";
	if ( time_h > 0 )
		time_string << time_h << "h ";
	if ( time_m > 0 )
		time_string << time_m << "m ";
	time_string << time_s << "s";

	return time_string.str();
}

string getRateString(boost::int64_t file_rate)
{
	ostringstream file_rate_string;
	file_rate_string << getFileSizeString(file_rate) << "/s";
	return file_rate_string.str();
}

string getFileSizeString(boost::int64_t file_size)
{
	ostringstream file_size_string;

	if (file_size <= 0)
		return string();

	if (file_size >= (1024 * 1024 * 1024))
		file_size_string <<  fixed << setprecision(3) << (file_size / 1024 / 1024 / 1024) << " GB";

	if (file_size >= (1024 * 1024) && file_size < (1024 * 1024 * 1024))
		file_size_string <<  fixed << setprecision(3) << (file_size / 1024 / 1024) << " MB";

	if (file_size >= 1024 && file_size < (1024 * 1024))
		file_size_string << fixed << setprecision(3) << (file_size / 1024) << " KB";

	if (file_size > 0 && file_size < 1024)
		file_size_string << file_size << " B";

	return file_size_string.str();
}

gt::Torrent::Torrent(string path) : m_path(path)
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

void gt::Torrent::setSavePath(string savepath)
{
	if (savepath.empty())
		savepath = gt::Core::getDefaultSavePath();
	if (savepath.empty())
		savepath = "./"; //Fall back to ./ if $HOME is not set
	m_torrent_params.save_path = savepath;
}

bool gt::Torrent::pollEvent(gt::Event &event)
{
	if (getTotalProgress() >= 100)
	{
		event.type = gt::Event::DownloadCompleted;
		return true;
	}

	return false;
}

string gt::Torrent::getTextState()
{
	ostringstream progress;
	int precision = 1;
	if (m_torrent_params.ti != NULL) //m_torrent_params.ti is not initial initialized for magnet links
		precision = m_torrent_params.ti->total_size() < 0x2000000;//Set 0 decimal places if file is less than 1 gig.
	progress << fixed << setprecision(precision) << getTotalProgress() << '%';
	return progress.str();
	
	const string stateText = { 
		"Queued for checking",
		"Checking...",
		"Downloading metadata...",
		progress.str(),
		"Finished",
		"Seeding...",
		"Allocating...",
		"Checking resume data..."
	};
	
	if(isPaused())
		return "Paused";
	
	return stateText[getState()];
}

float gt::Torrent::getTotalRatio()
{
	if ( getTotalDownloaded() > 0 )
		return float( getTotalUploaded() ) / float( getTotalDownloaded() );
	else
		return 0.0f;
}

string gt::Torrent::getTextTotalRatio()
{
	ostringstream ttr;
	ttr << fixed << setprecision(3) << getTotalRatio();
	return ttr.str();
}

void gt::Torrent::setPaused(bool isPaused)
{
	m_handle.auto_managed(!isPaused);
	if ( isPaused )
		m_handle.pause();
	else
		m_handle.resume();
}
