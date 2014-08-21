#include "Core.hpp"
#include "Event.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"
#include "Torrent.hpp"

#include <libtorrent.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/lazy_entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_info.hpp>

#define T_PPM 1000000.f

// format 0d 0h 0m 0s
std::string getTimeString(int64_t time_s)
{
	if(time_s <= 0 )
		return "∞";

	int64_t time_m = time_s / 60;
	time_s %= 60;
	int64_t time_h = time_m / 60;
	time_m %= 60;
	int64_t time_d = time_h / 24;
	time_h %= 24;

	std::ostringstream time_string;

	if ( time_d > 0 )
		time_string << time_d << "d ";
	if ( time_h > 0 )
		time_string << time_h << "h ";
	if ( time_m > 0 )
		time_string << time_m << "m ";
	time_string << time_s << "s";

	return time_string.str();
}

std::string getRateString(int64_t file_rate)
{
	return getFileSizeString(file_rate) + (file_rate ? "/s" : "");
}

std::string getFileSizeString(int64_t file_size)
{
	if (file_size <= 0)
		return std::string();

	std::ostringstream fss;
	fss << std::setprecision(3);

	if (file_size >= (1024 * 1024 * 1024))
		fss << file_size / (double)(1024 * 1024 * 1024) << " GB";
	else if (file_size >= (1024 * 1024))
		fss << (file_size / (double)(1024 * 1024)) << " MB";
	else if (file_size >= 1024)
		fss << (file_size / (double)1024) << " KB";
	else if (file_size > 0)
		fss << file_size << "B ";
	return fss.str();
}

gt::Torrent::Torrent(std::string path) : m_path(path)
{
	setSavePath(gt::Settings::settings["SavePath"]); //TODO add argument to allow user to override the default save path of $HOME/Downloads
	if (gt::Core::isLink(path))
		m_torrent_params.url = path;
	else
	{
		//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
		//Using decltype allows us to make it compatible with both versions while still properly using the constructor to avoid a compiler error on boost 1.55 when the = operator is used with a pointer.
		libtorrent::error_code ec;
		decltype(m_torrent_params.ti) tester = decltype(m_torrent_params.ti)(new libtorrent::torrent_info(path, ec));
		if (ec.value() == 0)
		{
			std::ifstream torrentcheck(path);
			bool isempty = torrentcheck.peek() == std::ifstream::traits_type::eof();
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

void gt::Torrent::setSavePath(std::string savepath)
{
	if (savepath.empty())
	{
		if(gt::Settings::optionExists(SAVEPATH_OPTION_KEY))
			savepath = gt::Settings::getOptionAsString(SAVEPATH_OPTION_KEY);
		else if(!gt::Platform::getDefaultSavePath().empty())
			savepath = gt::Platform::getDefaultSavePath();
		else
			savepath = "./"; //Fall back to ./ if $HOME is not set
	}
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

libtorrent::add_torrent_params gt::Torrent::getTorrentParams()
{
	return m_torrent_params;
}

libtorrent::torrent_handle& gt::Torrent::getHandle()
{
	return m_handle;
}

std::string& gt::Torrent::getPath()
{
	return m_path;
}

// Returns number of seconds the torrent has been active
int64_t gt::Torrent::getActiveTime()
{
	return m_handle.status().active_time;
}

// Returns formatted active time as string
std::string gt::Torrent::getTextActiveTime()
{
	return getTimeString(getActiveTime());
}

// Returns number of seconds eta for the torrent
int64_t gt::Torrent::getEta()
{
	return (getDownloadRate() <= 0) ? -1 : (getSize() / getDownloadRate());
}

// Returns formatted eta as string
std::string gt::Torrent::getTextEta()
{
	return getTimeString(getEta());
}

std::string gt::Torrent::getTextState()
{
	std::ostringstream o;
	int precision = 1;

	if(getHandle().status().queue_position != -1 && 
	   getHandle().status().queue_position >= stoi(gt::Settings::settings["ActiveDownloads"])) return "Queued";

	switch (getState())
	{
	case libtorrent::torrent_status::queued_for_checking:
		return "Queued for checking";
	case libtorrent::torrent_status::downloading_metadata:
		return "Downloading metadata...";
	case libtorrent::torrent_status::finished:
		return "Finished";
	case libtorrent::torrent_status::allocating:
		return "Allocating...";
	case libtorrent::torrent_status::checking_resume_data:
		return "Resuming...";
	case libtorrent::torrent_status::checking_files:
		return "Checking...";
	case libtorrent::torrent_status::seeding:
		return "Seeding";
	case libtorrent::torrent_status::downloading:
		break;
	}

	if(isPaused())
		return "Paused";

	if (m_torrent_params.ti != NULL) //m_torrent_params.ti is not initial initialized for magnet links
		precision = m_torrent_params.ti->total_size() < 0x2000000;//Set 0 decimal places if file is less than 1 gig.
	o << std::fixed << std::setprecision(precision) << getTotalProgress() << '%';
	return o.str();

}

float gt::Torrent::getTotalProgress()
{
	return ((float) getHandle().status().progress_ppm / 1000000.0f) * 100.0f;
}

unsigned int gt::Torrent::getUploadRate()
{
	return (isPaused() ? 0 : getHandle().status().upload_rate);
}

unsigned int gt::Torrent::getDownloadRate()
{
	return (isPaused() ? 0 : getHandle().status().download_rate);
}

unsigned int gt::Torrent::getPPMProgress()
{
	return getHandle().status().progress_ppm;
}

unsigned int gt::Torrent::getTotalSeeders()
{
	return getHandle().status().num_seeds;
}

unsigned int gt::Torrent::getTotalPeers()
{
	return getHandle().status().num_peers;
}

unsigned int gt::Torrent::getTotalLeechers()
{
	return getTotalPeers() - getTotalSeeders();
}

int64_t gt::Torrent::getTotalUploaded()
{
	return getHandle().status().total_upload;
}

int64_t gt::Torrent::getTotalDownloaded()
{
	return getHandle().status().total_download;
}

int64_t gt::Torrent::getSize()
{
	return getHandle().status().total_wanted;
}

int64_t gt::Torrent::getTimeRemaining()
{
	return (getDownloadRate() > 0) ? getSize() / getDownloadRate() : 0;
}

libtorrent::torrent_status::state_t gt::Torrent::getState()
{
	return m_handle.status().state;
}

std::string gt::Torrent::getCurrentTrackerURL()
{
	return m_handle.status().current_tracker;
}

std::string gt::Torrent::getTextUploadRate()
{
	return getRateString(getUploadRate());
}

std::string gt::Torrent::getTextDownloadRate()
{
	return getRateString(getDownloadRate());
}

std::string gt::Torrent::getTextTotalUploaded()
{
	return getFileSizeString(getTotalUploaded());
}

std::string gt::Torrent::getTextTotalDownloaded()
{
	return getFileSizeString(getTotalDownloaded());
}

std::string gt::Torrent::getTextSize()
{
	return getFileSizeString(getSize());
}

int64_t gt::Torrent::getRemaining()
{
	return getSize() - getTotalDownloaded();
}

std::string gt::Torrent::getTextRemaining()
{
	return getFileSizeString(getRemaining());
}

std::string gt::Torrent::getTextTimeRemaining()
{
	return getTimeString(getTimeRemaining());
}

bool gt::Torrent::isPaused()
{
	return getHandle().status().paused;
}

void gt::Torrent::setHandle(libtorrent::torrent_handle &h)
{
	m_handle = h;
}

void gt::Torrent::resume()
{
	setPaused(false);
}

void gt::Torrent::pause()
{
	setPaused(true);
}

std::string gt::Torrent::getName()
{
	return getHandle().status().name;
}

bool gt::Torrent::hasMetadata()
{
	return getHandle().status().has_metadata;
}

std::string gt::Torrent::getSavePath()
{
	return getHandle().status().save_path;
}

gt::Torrent::getInfoReturnType gt::Torrent::getInfo()
{
	return getHandle().torrent_file();
}

float gt::Torrent::getTotalRatio()
{
	if ( getTotalDownloaded() > 0 )
		return float( getTotalUploaded() ) / float( getTotalDownloaded() );
	else
		return 0.0f;
}

std::string gt::Torrent::getTextTotalRatio()
{
	std::ostringstream ttr;
	ttr << std::fixed << std::setprecision(3) << getTotalRatio();
	return ttr.str();
}

void gt::Torrent::setPaused(bool isPaused)
{
	m_handle.auto_managed(!isPaused);
	isPaused ? m_handle.pause() : m_handle.resume();
}

std::vector<bool> gt::Torrent::getPieces()
{
	libtorrent::bitfield p = m_handle.status().pieces;
	int n = m_handle.torrent_file()->num_pieces();
	std::vector<bool> pieces;
	for(int i = 0; i < n; ++i)
		pieces.push_back(p.get_bit(i));
	return pieces;
}

void gt::Torrent::setSequentialDownload(bool seq)
{
	getHandle().set_sequential_download(seq);
}

bool gt::Torrent::SequentialDownloadEnabled()
{
	return getHandle().status().sequential_download;
}

std::vector<std::string> gt::Torrent::filenames()
{
	std::vector<std::string> files;
	for(int i = 0; i < getInfo()->num_files(); ++i)
		files.push_back(getInfo()->files().file_path(i));
	return files;
}

std::string gt::Torrent::getFormattedHash()
{
	std::stringstream hash;
	for(auto val : m_handle.info_hash())
		hash << std::hex << (int)val;
	return hash.str();
}
