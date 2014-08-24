#pragma once

#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdint>
#include <functional>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>

#include "Peer.hpp"

std::string getTimeString(int64_t time_s);
std::string getRateString(int64_t file_rate);
std::string getFileSizeString(int64_t file_size);

namespace libtorrent
{
	struct add_torrent_params;
}

namespace gt
{
	struct Event;

	class Torrent
	{
	private:
		unsigned int m_id;
		libtorrent::add_torrent_params m_torrent_params;
		libtorrent::torrent_handle m_handle;
		std::string m_path;

	public:
		Torrent(std::string path);
		std::function<void(int, std::shared_ptr<Torrent>)> onStateChanged; // why does this remind me of kirby <('_')>
		bool pollEvent(gt::Event &event);
		void defaultCallback(int, std::shared_ptr<Torrent>);
		/* Think twice next time before mixing const correctness with inline */
		// Getters
		libtorrent::add_torrent_params getTorrentParams();

		libtorrent::torrent_handle& getHandle();

		std::string& getPath();

		// Returns number of seconds the torrent has been active
		int64_t getActiveTime();

		// Returns formatted active time as string
		std::string getTextActiveTime();

		// Returns number of seconds eta for the torrent
		int64_t getEta();

		// Returns formatted eta as string
		std::string getTextEta();

		// Returns a vector of bools for each piece, true if we have it, false otherwise
		std::vector<bool> getPieces();

		// Returns percentage of all files downloading
		float getTotalProgress();

		// Returns the current upload rate of the torrent
		unsigned int getUploadRate();

		// Returns the current download rate of the torrent
		unsigned int getDownloadRate();

		// Returns the progress in PPM of all files downloading in this torrent
		unsigned int getPPMProgress();

		// Returns the current number of seeders attached to the file
		unsigned int getTotalSeeders();

		// Returns the current number of peers attached to the file
		unsigned int getTotalPeers();

		// Returns the current number of leechers attached to the file
		unsigned int getTotalLeechers();

		// Returns the current amount of data uploaded for this torrent
		int64_t getTotalUploaded();

		// Returns the current amount of data downloaded for this torrent
		int64_t getTotalDownloaded();

		// Returns the total size of files in this torrent
		int64_t getSize();

		//Returns the elapsed time remaining in seconds
		int64_t getTimeRemaining();

		// Returns the ratio (uploaded/downloaded) for this torrent
		float getTotalRatio();

		// Returns the current torrent state (downloading, queueing, seeding, etc)
		libtorrent::torrent_status::state_t getState();

		//Returns the URL of the last working tracker
		std::string getCurrentTrackerURL();

		//Force a recheck of the torrent
		void torrentForceRecheck();

		// Returns a friendly string for the torrent state
		std::string getTextState();

		// Returns a friendly string for the current upload rate
		std::string getTextUploadRate();

		// Returns a friendly string for the current download rate
		std::string getTextDownloadRate();

		// Returns a friendly string for the current upload total
		std::string getTextTotalUploaded();

		// Returns a friendly string for the current download total
		std::string getTextTotalDownloaded();

		// Returns a friendly string for the total size of files in torrent
		std::string getTextSize();

		// Returns a the total size of files remain
		int64_t getRemaining();

		// Returns a friendly string for the total size of files remaining to download in torrent
		std::string getTextRemaining();

		// Returns a friendly string for the current ratio
		std::string getTextTotalRatio();

		// Returns a friendly string for the current time remaining
		std::string getTextTimeRemaining();

		bool isPaused();

		// Setters
		void setHandle(libtorrent::torrent_handle &h);

		void setSavePath(std::string savepath);

		void setPaused(bool isPaused);

		void resume();

		void pause();

		std::string getName();

		bool hasMetadata();

		std::string getSavePath();

		std::string getFormattedHash();

		//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
		//Using decltype allows us to make it compatible with both versions.
		typedef decltype(boost::const_pointer_cast<const libtorrent::torrent_info>(m_torrent_params.ti)) getInfoReturnType;
		getInfoReturnType getInfo();

		void setSequentialDownload(bool seq);
		bool SequentialDownloadEnabled();
		std::vector<std::string> filenames();

		std::vector<Peer> getPeers();
	};
}
