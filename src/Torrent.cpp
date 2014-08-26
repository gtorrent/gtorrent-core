#include "Core.hpp"
#include "Event.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "Settings.hpp"
#include "Torrent.hpp"
#include <cmath>
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

	if (time_d > 0)
		time_string << time_d << "d ";
	if (time_h > 0)
		time_string << time_h << "h ";
	if (time_m > 0)
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
	std::ostringstream fss;

	if (file_size <= 0)
		return std::string();

	fss << std::setprecision(3);
	std::string units[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
	int e = ::floor(::log(file_size) / ::log(1024));
	fss << (file_size / ::pow(1024, e)) << " " << units[e];
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

//	onStateChanged = [](int i, shared_ptr<Torrent> j) { std::cout << "State Changed ! Old state was " << i << ", new state is " << j->status().state << std::endl; }; // default handler
	onStateChanged = std::bind(&gt::Torrent::defaultCallback, this, std::placeholders::_1, std::placeholders::_2);
	/*
	 * To use, for example in GtkMainWindow, let's say tor is a shared_ptr, you would write
	 * tor->onStateChanged = [](int oldstate, shared_ptr<gt::Torrent> sender) {...}
	 * oldstate is just the state_t enum, you can't use it to determine if the torrent was paused
	 * sender is the ptr to the torrent that just changed state.
	 * You can use std::bind to make it call a member of your class instead of lambdas:
	 * tor->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, placeholders::_1, placeholders::_2);
	 * where torrentStateChangedCallback must take an int parameter and shared_ptr<gt::Torrent> parameter
	 * Yup placeholders are a special kind of disgusting but even boost has them so it's either that or add deps on libsigc
	 */
}

void gt::Torrent::defaultCallback(int i, std::shared_ptr<gt::Torrent> j)
{
	/* Do something */
}

void gt::Torrent::setSavePath(std::string savepath)
{
	if (savepath.empty())
	{
		if(gt::Settings::optionExists(SAVEPATH_OPTION_KEY))
			savepath = gt::Settings::settings[SAVEPATH_OPTION_KEY];
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

libtorrent::torrent_handle& gt::Torrent::getHandle() const
{
	return *dynamic_cast<libtorrent::torrent_handle*>(const_cast<gt::Torrent*>(this));
}

std::string& gt::Torrent::getPath()
{
	return m_path;
}

// Returns number of seconds the torrent has been active
int64_t gt::Torrent::getActiveTime()
{
	return status().active_time;
}

// Returns formatted active time as string
std::string gt::Torrent::getTextActiveTime()
{
	return getTimeString(status().active_time);
}

// Returns number of seconds eta for the torrent
int64_t gt::Torrent::getEta()
{
	return (getDownloadRate() <= 0) ? -1 : (status().total_wanted / getDownloadRate());
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

	if(status().queue_position != -1 && 
	   status().queue_position >= stoi(gt::Settings::settings["ActiveDownloads"])) return "Queued";

	switch (status().state)
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
	return ((float) status().progress_ppm / 1000000.0f) * 100.0f;
}

unsigned int gt::Torrent::getUploadRate()
{
	return (isPaused() ? 0 : status().upload_rate);
}

unsigned int gt::Torrent::getDownloadRate()
{
	return (isPaused() ? 0 : status().download_rate);
}

unsigned int gt::Torrent::getPPMProgress()
{
	return status().progress_ppm;
}

unsigned int gt::Torrent::getTotalSeeders()
{
	return status().num_seeds;
}

unsigned int gt::Torrent::getTotalPeers()
{
	return status().num_peers;
}

unsigned int gt::Torrent::getTotalLeechers()
{
	return status().num_peers - status().num_seeds;
}

int64_t gt::Torrent::getTotalUploaded()
{
	return status().total_upload;
}

int64_t gt::Torrent::getTotalDownloaded()
{
	return status().total_download;
}

int64_t gt::Torrent::getSize()
{
	return status().total_wanted;
}

int64_t gt::Torrent::getTimeRemaining()
{
	return (getDownloadRate() > 0) ? status().total_wanted / getDownloadRate() : 0;
}

libtorrent::torrent_status::state_t gt::Torrent::getState()
{
	return status().state;
}

std::string gt::Torrent::getCurrentTrackerURL()
{
	return status().current_tracker;
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
	return getFileSizeString(status().total_upload);
}

std::string gt::Torrent::getTextTotalDownloaded()
{
	return getFileSizeString(status().total_download);
}

std::string gt::Torrent::getTextSize()
{
	return getFileSizeString(status().total_wanted);
}

int64_t gt::Torrent::getRemaining()
{
	return status().total_wanted - status().total_download;
}

std::string gt::Torrent::getTextRemaining()
{
	return getFileSizeString(status().total_wanted - status().total_download);
}

std::string gt::Torrent::getTextTimeRemaining()
{
	return getTimeString((getDownloadRate() > 0) ? status().total_wanted / getDownloadRate() : 0);
}

bool gt::Torrent::isPaused()
{
	return status().paused;
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
	return status().name;
}

bool gt::Torrent::hasMetadata()
{
	return status().has_metadata;
}

std::string gt::Torrent::getSavePath()
{
	return status().save_path;
}

gt::Torrent::getInfoReturnType gt::Torrent::getInfo()
{
	return torrent_file();
}

float gt::Torrent::getTotalRatio()
{
	if (status().total_download > 0)
		return float(status().total_upload) / float(status().total_download);
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
	auto_managed(!isPaused);
	isPaused ? pause() : resume();
}

std::vector<bool> gt::Torrent::getPieces()
{
	std::vector<bool> pieces;
	if(!status().has_metadata) return pieces;
	libtorrent::bitfield p = status().pieces;
	int n = torrent_file()->num_pieces();
	for(int i = 0; i < n; ++i)
		pieces.push_back(p.get_bit(i));
	return pieces;
}

void gt::Torrent::setSequentialDownload(bool seq)
{
	set_sequential_download(seq);
}

bool gt::Torrent::SequentialDownloadEnabled()
{
	return status().sequential_download;
}

std::vector<std::string> gt::Torrent::filenames()
{
	std::vector<std::string> files;
	if(!torrent_file()) return files;
	for(int i = 0; i < torrent_file()->num_files(); ++i)
		files.push_back(torrent_file()->files().file_path(i));
	return files;
}

std::string gt::Torrent::getFormattedHash()
{
	std::stringstream hash;
	for(auto val : info_hash())
		hash << std::hex << (int)val;

	return hash.str();
}

std::vector<gt::Peer> gt::Torrent::getPeers()
{
	std::vector<gt::Peer> result;
	std::vector<libtorrent::peer_info> peers;
	try
	{
	getHandle().get_peer_info(peers);
	for (auto p : peers)
		result.push_back(gt::Peer(p));
	return result;
	}
	catch(...)
	{
		return std::vector<gt::Peer>();
	}
}
