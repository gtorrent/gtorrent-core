#pragma once

#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>

std::string getTimeString(boost::int64_t time_s);
std::string getRateString(boost::int64_t file_rate);
std::string getFileSizeString(boost::int64_t file_size);

namespace libtorrent
{
	class add_torrent_params;
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

		bool pollEvent(gt::Event &event);

		// Getters
		inline libtorrent::add_torrent_params getTorrentParams()
		{
			return m_torrent_params;
		}
		inline libtorrent::torrent_handle& getHandle()
		{
			return m_handle;
		}
		inline std::string& getPath()
		{
			return m_path;
		}

		// Returns number of seconds the torrent has been active
		inline boost::int getActiveTime()
		{
			return m_handle.status().active_time;
		}

		// Returns number of seconds eta for the torrent
		inline boost::int64_t getEta()
		{
			return (getDownloadRate() <= 0) ? -1 : (getWanted() / getDownloadRate());
		}

		// Returns a vector of bools for each piece, true if we have it, false otherwise
		std::vector<bool> getPieces();

		// Returns percentage of all files downloading
		inline float getTotalProgress()
		{
			return (float) getHandle().status().progress_ppm / 10000.0f;
		}

		// Returns the current upload rate of the torrent
		inline unsigned int getUploadRate()
		{
			return (isPaused() ? 0 : getHandle().status().upload_rate);
		}

		// Returns the current download rate of the torrent
		inline unsigned int getDownloadRate()
		{
			return (isPaused() ? 0 : getHandle().status().download_rate);
		}

		// Returns the progress in PPM of all files downloading in this torrent
		inline unsigned int getPPMProgress()
		{
			return getHandle().status().progress_ppm;
		}

		// Returns the current number of seeders attached to the file
		inline unsigned int getNumSeeders()
		{
			return getHandle().status().num_seeds;
		}

		// Returns the current number of peers connected to the torrent
		inline unsigned int getNumPeers()
		{
			return getHandle().status().num_peers;
		}

		// Returns the current number of leechers connected to the torrent
		inline unsigned int getTotalLeechers()
		{
			return getTotalPeers() - getTotalSeeders();
		}

		// Returns the current amount of data uploaded for this torrent, in bytes
		inline boost::int64_t getTotalUploaded()
		{
			return getHandle().status().total_upload;
		}

		// Returns the current amount of data downloaded for this torrent, in bytes
		inline boost::int64_t getTotalDownloaded()
		{
			return getHandle().status().total_download;
		}

		// Returns the total size of files in this torrent, in bytes
		inline boost::int64_t getSize()
		{
			return getHandle().status().total_wanted;
		}

		// Returns the elapsed time remaining in seconds
		inline boost::int64_t getTimeRemaining()
		{
			return (getDownloadRate() > 0) ? getTorrentSize() / getDownloadRate() : 0;
		}

		// Returns the ratio (uploaded/downloaded) for this torrent
		float getTotalRatio();

		// Returns the current torrent state (downloading, queueing, seeding, etc)
		inline libtorrent::torrent_status::state_t getState()
		{
			return m_handle.status().state;
		}

		// Returns the URL of the last working tracker
		inline std::string getCurrentTrackerURL()
		{
			return m_handle.status().current_tracker;
		}

		// Returns a the total size of files remaining to download in torrent
		inline boost::int64_t getRemaining()
		{
			return getHandle().status().total_wanted - getHandle().status().total_wanted_done;
		}

		// Returns true if the torrent is paused, otherwise returns false.
		inline bool isPaused()
		{
			return getHandle().status().paused;
		}

		// Returns the name for this torrent
		inline std::string getName()
		{
			return getHandle().status().name;
		}

		// Returns the filename for this torrent
		inline std::string getTorrentFileNameString()
		{
			return getHandle().status().torrent_file;
		}

		/*
		inline std::storage_mode_t getStorageMode()
		{
			return getHandle().status().storage_mode;
		}
		*/

		// Returns a value in the range [0, 1],
		// that represents the progress of the torrent's current task.
		// It may be checking files or downloading.
		inline std::float getProgress()
		{
			return getHandle().status().progress;
		}

		//Returns the number of peers that have finished downloading
		inline std::int getNumComplete()
		{
			return getHandle().status().num_complete;
		}

		// Returns the number of peers that have not yet finished downloading
		inline std::int getNumIncomplete()
		{
			return getHandle().status().num_incomplete;
		}

		// Returns the number of seeds in our peer list
		inline std::int getListSeeds()
		{
			return getHandle().status().list_seeds;
		}

		// Returns the total number of peers (including seeds)
		// We are not necessarily connected to all the peers in our peer list
		// This is the number of peers we know of in total,
		// including banned peers and peers that we have failed to connect to
		inline std::int getListPeers()
		{
			return getHandle().status().list_peers;
		}

		// Returns the number of peers that are suitable to be connected to
		inline std::int getConnectCandidates()
		{
			return getHandle().status().connect_candidates;
		}

		//Returns the number of distributed copies of the file
		//Note that one copy may be spread out among many peers.
		inline std::float getDistributedCopies()
		{
			return getHandle().status().distributed_copies;
		}

		//Returns the number of unchoked peers in this torrent
		inline std::int getNumUploads()
		{
			return getHandle().status().num_uploads;
		}

		//Returns the number of peer connections this torrent has,
		//including half-open connections that haven't completed
		//the bittorrent handshake yet.
		inline std::int getNumConnections()
		{
			return getHandle().status().num_connections;
		}

		//Returns the number of seconds this torrent has been seeding for
		inline std::int getSeedingTime()
		{
			return getHandle().status().seeding_time;
		}

		// Returns a rank of how important it is to seed the torrent,
		// used to determine which torrents to seed and which to queue
		inline std::int getSeedRank()
		{
			return getHandle().status().seed_rank;
		}

		// Returns the number of seconds since this torrent acquired scrape data
		// If it has never done that, this value is -1
		inline std::int getLastScrape()
		{
			return getHandle().status().last_scrape;
		}
		inline std::int getSparseRegions()
		{
			return getHandle().status().sparse_regions;
		}

		// Returns the priority assigned to this torrent
		inline std::int getPriority()
		{
			return getHandle().status().priority;
		}

		// Returns true if the IP filter applies to this torrent
		inline std::bool getIpFilterApplies()
		{
			return getHandle().status().ip_filter_applies;
		}

		// Returns true if this torrent is being automatically managed
		inline std::bool getAutoManaged()
		{
			getHandle().status().auto_managed;
		}

		// Returns true if this torrent is set to download sequentially
		inline std::bool getSequentialDownload()
		{
			getHandle().status().sequential_download;
		}

		// Returns true if this torrent is seeding
		inline std::bool getIsSeeding()
		{
			return getHandle().status().is_seeding;
		}

		// Returns true if this torrent has finished downloading
		inline std::bool getIsFinished()
		{
			return getHandle().status().is_finished;
		}

		// Returns true if this torrent has metadata
		inline std::bool getHasMetadata()
		{
			return getHandle().status().has_metadata;
		}

		// Returns true if this torrent has incoming data
		inline std::bool getHasIncoming()
		{
			return getHandle().status().has_incoming;
		}

		// Returns the info hash for this torrent
		inline std::sha1_hash getInfoHash()
		{
			return getHandle().status().info_hash;
		}

		// Returns the path to which this torrent is / will be saved
		inline std::string getSavePath()
		{
			return getHandle().status().save_path;
		}

		//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
		//Using decltype allows us to make it compatible with both versions while still properly using the constructor to avoid a compiler error on boost 1.55 when the = operator is used with a pointer.
		//Sorry for the terrible hack, TODO: Find better detection method to cast constness to libtorrent:torrent_info inside shared/intrusive pointer
		#define getInfoReturnType std::conditional<std::is_same<decltype(m_torrent_params.ti), boost::shared_ptr<libtorrent::torrent_info>>::value, boost::shared_ptr<const libtorrent::torrent_info>, boost::intrusive_ptr<const libtorrent::torrent_info>>::type
		inline getInfoReturnType getInfo()
		{
			return getHandle().torrent_file();
		}

		//Force a recheck of the torrent
		void torrentForceRecheck();

		// Setters

		// Sets the handle for this torrent
		inline void setHandle(libtorrent::torrent_handle &h)
		{
			m_handle = h;
		}

		// Sets the save path for this torrent
		void setSavePath(std::string savepath);

		// Sets the paused status for this torrent
		void setPaused(bool isPaused);

		// Pauses this torrent
		inline void pause()
		{
			setPaused(true);
		}
		
		// Un-pauses this torrent
		inline void resume()
		{
			setPaused(false);
		}

		// Sets this torrent to download sequentially
		void setSequentialDownload(bool seq);

		bool SequentialDownloadEnabled();
		std::vector<std::string> filenames();
	};
}
