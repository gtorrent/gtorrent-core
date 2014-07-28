#pragma once

using namespace std;

#include "libtorrent.hpp"
#include "Event.hpp"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

string getTimeString(boost::int64_t time_s);
string getRateString(boost::int64_t file_rate);
string getFileSizeString(boost::int64_t file_size);

namespace gt {
	class Torrent
	{
	private:
		unsigned int m_id;
		libtorrent::add_torrent_params m_torrent_params;
		libtorrent::torrent_handle m_handle;
		string m_path;

	public:
		Torrent(string path);

		bool pollEvent(gt::Event &event);

		/* Think twice next time before mixing const correctness with inline */
		// Getters
		inline libtorrent::add_torrent_params getTorrentParams() { return m_torrent_params; }
		inline libtorrent::torrent_handle& getHandle() { return m_handle; }
		inline string& getPath() { return m_path; }

		// Returns number of seconds the torrent has been active
		inline boost::int64_t getActiveTime() { return m_handle.status().active_time; }

		// Returns formatted active time as string
		inline string getTextActiveTime() { return getTimeString(getActiveTime()); }

		// Returns number of seconds eta for the torrent
		inline boost::int64_t getEta() { return (getDownloadRate() <= 0) ? -1 : (getWanted() / getDownloadRate()); }

		// Returns formatted eta as string
		inline string getTextEta() { return getTimeString(getEta()); }

        // Returns a vector of bools for each piece, true if we have it, false otherwise
        vector<bool> getPieces();

		// Returns percentage of all files downloading
		inline float getTotalProgress() { return ((float) getHandle().status().progress_ppm / 1000000.0f) * 100.0f; }

		// Returns the current upload rate of the torrent
		inline unsigned int getUploadRate() { return (isPaused() ? 0 : getHandle().status().upload_rate); }

		// Returns the current download rate of the torrent
		inline unsigned int getDownloadRate() { return (isPaused() ? 0 : getHandle().status().download_rate); }

		// Returns the progress in PPM of all files downloading in this torrent
		inline unsigned int getPPMProgress() { return getHandle().status().progress_ppm; }

		// Returns the current number of seeders attached to the file
		inline unsigned int getTotalSeeders() { return getHandle().status().num_seeds; }

		// Returns the current number of peers attached to the file
		inline unsigned int getTotalPeers() { return getHandle().status().num_peers; }

		// Returns the current number of leechers attached to the file
		inline unsigned int getTotalLeechers() { return getTotalPeers() - getTotalSeeders(); }

		// Returns the current amount of data uploaded for this torrent
		inline boost::int64_t getTotalUploaded() { return getHandle().status().total_upload; }

		// Returns the current amount of data downloaded for this torrent
		inline boost::int64_t getTotalDownloaded() { return getHandle().status().total_download; }

		// Returns the total size of files in this torrent
		inline boost::int64_t getSize() { return getHandle().status().total_wanted; }

		// Returns the total size of wanted files in this torrent
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getWanted() { return getSize(); }

		//Returns the size of the torrent
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getTorrentSize() { return getSize(); }

		//Returns the elapsed time remaining in seconds
		inline boost::int64_t getTimeRemaining() { return (getDownloadRate() > 0) ? getTorrentSize() / getDownloadRate() : 0; }

		// Returns the ratio (uploaded/downloaded) for this torrent
		float getTotalRatio();

		// Returns the current torrent state (downloading, queueing, seeding, etc)
		inline libtorrent::torrent_status::state_t getState() { return m_handle.status().state; }

		//Returns the URL of the last working tracker
		inline string getCurrentTrackerURL() { return m_handle.status().current_tracker; }

		//Force a recheck of the torrent
		void torrentForceRecheck();

		// Returns a friendly string for the torrent state
		string getTextState();

		// Returns a friendly string for the current upload rate
		inline string getTextUploadRate() { return getRateString(getUploadRate()); }

		// Returns a friendly string for the current download rate
		inline string getTextDownloadRate() { return getRateString(getDownloadRate()); }

		// Returns a friendly string for the current upload total
		inline string getTextTotalUploaded() { return getFileSizeString(getTotalUploaded()); }

		// Returns a friendly string for the current download total
		inline string getTextTotalDownloaded() { return getFileSizeString(getTotalDownloaded()); }

		// Returns a friendly string for the total size of files in torrent
		inline string getTextSize() { return getFileSizeString(getSize()); }

		// Returns a the total size of files remaining to download in torrent
		inline boost::int64_t getRemaining() { return getSize() - getTotalDownloaded(); }

		// Returns a friendly string for the total size of files remaining to download in torrent
		inline string getTextRemaining() { return getFileSizeString(getRemaining()); }

		// Returns a friendly string for the current ratio
		string getTextTotalRatio();

		// Returns a friendly string for the current time remaining
		inline string getTextTimeRemaining() { return getTimeString(getTimeRemaining()); }

		inline bool isPaused() { return getHandle().status().paused; }

		// Setters
		inline void setHandle(libtorrent::torrent_handle &h) { m_handle = h; }
		void setSavePath(string savepath);

		void setPaused(bool isPaused);

		inline void resume() { setPaused(false); }
		inline void pause() { setPaused(true); }
	};
}
