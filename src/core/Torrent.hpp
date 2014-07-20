#pragma once

#include "libtorrent.hpp"
#include "Event.hpp"
#include <iomanip>
#include <sstream>
#include <fstream>

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

	// Getters
	inline libtorrent::add_torrent_params getTorrentParams() const { return m_torrent_params; };
	inline libtorrent::torrent_handle& getHandle() { return m_handle; };
	inline const string& getPath() const { return m_path; };

	// Returns number of seconds the torrent has been active
	// TODO: Rename this to getActiveTime()
	inline boost::int64_t getActive() const { return m_handle.status().active_time; };

	// Returns formatted active time as string
	// TODO: Rename this to getTextActiveTime()
	inline string getTextActive() const { return getTimeString( getActive() ); };

	// Returns number of seconds eta for the torrent
	inline boost::int64_t getEta() const { return ( getDownloadRate() <= 0 ) ? -1 : ( getWanted() / getDownloadRate() ); };

	// Returns formatted eta as string
	inline string getTextEta() const { return getTimeString( getEta() ); };

	// Returns percentage of all files downloading
	inline float getTotalProgress() const { return ( ( float ) getHandle().status().progress_ppm / 1000000.0f ) * 100.0f; };

	// Returns the current upload rate of the torrent
	inline unsigned int getUploadRate() const { return getHandle().status().upload_rate; };

	// Returns the current download rate of the torrent
	inline unsigned int getDownloadRate() const { return getHandle().status().download_rate; };

	// Returns the progress in PPM of all files downloading in this torrent
	inline unsigned int getPPMProgress() const { return getHandle().status().progress_ppm; };

	// Returns the current number of seeders attached to the file
	inline unsigned int getTotalSeeders() const { return getHandle().status().num_seeds; };

	// Returns the current number of peers attached to the file
	inline unsigned int getTotalPeers() const { return getHandle().status().num_peers; };

	// Returns the current number of leechers attached to the file
	inline unsigned int getTotalLeechers() const { return getTotalPeers() - getTotalSeeders(); };

	// Returns the current amount of data uploaded for this torrent
	inline boost::int64_t getTotalUploaded() const { return getHandle().status().total_upload; };

	// Returns the current amount of data downloaded for this torrent
	inline boost::int64_t getTotalDownloaded() const { return getHandle().status().total_download; };

	// Returns the total size of files in this torrent
	inline boost::int64_t getSize() const { return getHandle().status().total_wanted; };

	// Returns the total size of wanted files in this torrent
	// TODO: Remove this, duplicate of getSize()
	inline boost::int64_t getWanted() const { return getSize(); };

	//Returns the size of the torrent
	// TODO: Remove this, duplicate of getSize()
	inline boost::int64_t getTorrentSize() const { return getSize(); };

	//Returns the elapsed time remaining in seconds
	inline boost::int64_t getTimeRemaining() const { return ( getDownloadRate() > 0 ) ? getTorrentSize() / getDownloadRate() : 0; };

	// Returns the ratio (uploaded/downloaded) for this torrent
	float getTotalRatio() const;

	// Returns the current torrent state (downloading, queueing, seeding, etc)
	inline libtorrent::torrent_status::state_t getState() const { return m_handle.status().state; };

	//Returns the URL of the last working tracker
	inline string getCurrentTrackerURL() const { return m_handle.status().current_tracker; };

	//Force a recheck of the torrent
	void torrentForceRecheck();

	// Returns a friendly string for the torrent state
	string getTextState();

	// Returns a friendly string for the current upload rate
	inline string getTextUploadRate() const { return getRateString( getUploadRate() ); };

	// Returns a friendly string for the current download rate
	inline string getTextDownloadRate() const { return getRateString( getDownloadRate() ); };

	// Returns a friendly string for the current upload total
	inline string getTextTotalUploaded() const { return getFileSizeString( getTotalUploaded() ); };

	// Returns a friendly string for the current download total
	inline string getTextTotalDownloaded() const { return getFileSizeString( getTotalDownloaded() ); };

	// Returns a friendly string for the total size of files in torrent
	inline string getTextSize() const { return getFileSizeString( getSize() ); };

	// Returns a the total size of files remaining to download in torrent
	inline boost::int64_t getRemaining() const { return m_handle.status().total_wanted - m_handle.status().total_wanted_done; };

	// Returns a friendly string for the total size of files remaining to download in torrent
	inline string getTextRemaining() const { return getFileSizeString( getRemaining() ); };

	// Returns a friendly string for the current ratio
	string getTextTotalRatio();

	// Returns a friendly string for the current time remaining
	inline string getTextTimeRemaining() const { return getTimeString( getTimeRemaining() ); };

	inline bool isPaused() const { return getHandle().status().paused; };

	// Setters
	inline void setHandle(libtorrent::torrent_handle &h) { m_handle = h; };
	void setSavePath(string savepath);

	void setPaused(bool isPaused);

	inline void resume() { setPause( false ); };
	inline void pause() { setPause( true ); };
};
