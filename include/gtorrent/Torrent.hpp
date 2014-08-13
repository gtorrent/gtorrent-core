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

		/* Think twice next time before mixing const correctness with inline */
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
		inline boost::int64_t getActiveTime()
		{
			return m_handle.status().active_time;
		}

		// Returns formatted active time as string
		inline std::string getActiveTimeString()
		{
			return getTimeString(getActiveTime());
		}

		// Returns number of seconds eta for the torrent
		inline boost::int64_t getEta()
		{
			return (getDownloadRate() <= 0) ? -1 : (getWanted() / getDownloadRate());
		}

		// Returns formatted eta as string
		inline std::string getEtaString()
		{
			if (getHandle().status().progress_ppm == 1000000)
			{
				return "∞";
			}
			else
			{
				return getTimeString(getEta());
			}
		}

		// Returns a vector of bools for each piece, true if we have it, false otherwise
		std::vector<bool> getPieces();

		// Returns percentage of all files downloading
		inline float getTotalProgress()
		{
			return ((float) getHandle().status().progress_ppm / 1000000.0f) * 100.0f;
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
		inline unsigned int getTotalSeeders()
		{
			return getHandle().status().num_seeds;
		}

		// Returns the current number of peers attached to the file
		inline unsigned int getTotalPeers()
		{
			return getHandle().status().num_peers;
		}

		// Returns the current number of leechers attached to the file
		inline unsigned int getTotalLeechers()
		{
			return getTotalPeers() - getTotalSeeders();
		}

		// Returns the current amount of data uploaded for this torrent
		inline boost::int64_t getTotalUploaded()
		{
			return getHandle().status().total_upload;
		}

		// Returns the current amount of data downloaded for this torrent
		inline boost::int64_t getTotalDownloaded()
		{
			return getHandle().status().total_download;
		}

		// Returns the total size of files in this torrent
		inline boost::int64_t getSize()
		{
			return getHandle().status().total_wanted;
		}

		// Returns the total size of wanted files in this torrent
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getWanted()
		{
			return getSize();
		}

		//Returns the size of the torrent
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getTorrentSize()
		{
			return getSize();
		}

		//Returns the elapsed time remaining in seconds
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

		//Returns the URL of the last working tracker
		inline std::string getCurrentTrackerURL()
		{
			return m_handle.status().current_tracker;
		}

		//Force a recheck of the torrent
		void torrentForceRecheck();

		// Returns a friendly string for the torrent state
		std::string getStateString();

		// Returns a friendly string for the current upload rate
		inline std::string getUploadRateString()
		{
			return getRateString(getUploadRate());
		}

		// Returns a friendly string for the current download rate
		inline std::string getDownloadRateString()
		{
			return getRateString(getDownloadRate());
		}

		// Returns a friendly string for the current upload total
		inline std::string getTotalUploadedString()
		{
			return getFileSizeString(getTotalUploaded());
		}

		// Returns a friendly string for the current download total
		inline std::string getTotalDownloadedString()
		{
			return getFileSizeString(getTotalDownloaded());
		}

		// Returns a friendly string for the total size of files in torrent
		inline std::string getSizeString()
		{
			return getFileSizeString(getSize());
		}

		// Returns a the total size of files remaining to download in torrent
		inline boost::int64_t getRemaining()
		{
			//return getSize() - getTotalDownloaded();
			return getHandle().status().total_wanted - getHandle().status().total_wanted_done;
		}

		// Returns a friendly string for the total size of files remaining to download in torrent
		inline std::string getRemainingString()
		{
			return getFileSizeString(getRemaining());
		}

		// Returns a friendly string for the current ratio
		std::string getTotalRatioString();

		// Returns a friendly string for the current time remaining
		inline std::string getTimeRemainingString()
		{
			return getTimeString(getTimeRemaining());
		}

		inline bool isPaused()
		{
			return getHandle().status().paused;
		}

		// Setters
		inline void setHandle(libtorrent::torrent_handle &h)
		{
			m_handle = h;
		}
		void setSavePath(std::string savepath);

		void setPaused(bool isPaused);

		inline void resume()
		{
			setPaused(false);
		}

		inline void pause()
		{
			setPaused(true);
		}

		inline std::string getName()
		{
			return getHandle().status().name;
		}
		inline std::string getTorrentFileNameString()
		{
			//return std::string(getHandle().status().torrent_file);
			std::stringstream tfn;
			tfn << "torrent-filename.torrent";
			return tfn.str();
		}

		inline std::string getCurrentTrackerString()
		{
			return getHandle().status().current_tracker;
		}

		inline std::string getWantedDoneString()
		{
			//size_type total_wanted_done;
			return getFileSizeString(getHandle().status().total_wanted_done);
		}
		inline std::string getTotalWantedString()
		{
			//size_type total_wanted;
			return getFileSizeString(getHandle().status().total_wanted);
		}
		inline std::string getAllTimeUploadString()
		{
			////size_type all_time_upload;
			return getFileSizeString(getHandle().status().all_time_upload);
		}
		inline std::string getAllTimeDownloadString()
		{
			//size_type all_time_download;
			return getFileSizeString(getHandle().status().all_time_download);
		}
		inline std::string getStorageModeString()
		{
			std::stringstream sm;
			sm << getHandle().status().storage_mode;
			return sm.str();
		}
		inline std::string getProgressString()
		{
			std::stringstream p;
			p << getHandle().status().progress;
			return p.str();
		}
		inline std::string getProgressPpmString()
		{
			std::stringstream pp;
			pp << getHandle().status().progress_ppm;
			return pp.str();
		}

		inline std::string getNumSeedsString()
		{
			std::stringstream ns;
			ns << getHandle().status().num_seeds;
			return ns.str();
		}
		inline std::string getNumPeersString()
		{
			std::stringstream np;
			np << getHandle().status().num_peers;
			return np.str();
		}
		inline std::string getNumCompleteString()
		{
			std::stringstream nc;
			nc << getHandle().status().num_complete;
			return nc.str();
		}
		inline std::string getNumIncompleteString()
		{
			std::stringstream ni;
			ni << getHandle().status().num_incomplete;
			return ni.str();
		}
		inline std::string getListSeedsString()
		{
			std::stringstream ls;
			ls << getHandle().status().list_seeds;
			return ls.str();
		}
		inline std::string getListPeersString()
		{
			std::stringstream lp;
			lp << getHandle().status().list_peers;
			return lp.str();
		}
		inline std::string getConnectCandidatesString()
		{
			std::stringstream cc;
			cc << getHandle().status().connect_candidates;
			return cc.str();
		}
		inline std::string getDistributedCopiesString()
		{
			std::stringstream dc;
			dc << getHandle().status().distributed_copies;
			return dc.str();
		}

		inline std::string getNumUploadsString()
		{
			std::stringstream nu;
			nu << getHandle().status().num_uploads;
			return nu.str();
		}
		inline std::string getNumConnectionsString()
		{
			std::stringstream nc;
			nc << getHandle().status().num_connections;
			return nc.str();
		}
		inline std::string getSeedingTimeString()
		{
			return getTimeString(getHandle().status().seeding_time);
		}
		inline std::string getSeedRankString()
		{
			std::stringstream sr;
			sr << getHandle().status().seed_rank;
			return sr.str();
		}
		inline std::string getLastScrapeString()
		{
			return getTimeString(getHandle().status().last_scrape);
		}
		inline std::string getSparseRegionsString()
		{
			std::stringstream sr;
			sr << getHandle().status().sparse_regions;
			return sr.str();
		}
		inline std::string getPriorityString()
		{
			std::stringstream p;
			p << getHandle().status().priority;
			return p.str();
		}
		inline std::string getIpFilterAppliesString()
		{
			std::stringstream ifa;
			ifa << getHandle().status().ip_filter_applies;
			return ifa.str();
		}
		inline std::string getPausedString()
		{
			std::stringstream pa;
			pa << getHandle().status().paused;
			return pa.str();
		}
		inline std::string getAutoManagedString()
		{
			std::stringstream am;
			am << getHandle().status().auto_managed;
			return am.str();
		}
		inline std::string getSequentialDownloadString()
		{
			std::stringstream sd;
			sd << getHandle().status().sequential_download;
			return sd.str();
		}
		inline std::string getIsSeedingString()
		{
			std::stringstream ise;
			ise << getHandle().status().is_seeding;
			return ise.str();
		}
		inline std::string getIsFinishedString()
		{
			std::stringstream ifi;
			ifi << getHandle().status().is_finished;
			return ifi.str();
		}
		inline bool hasMetadata()
		{
			return getHandle().status().has_metadata;
		}
		inline std::string getHasMetadataString()
		{
			std::stringstream hm;
			hm << getHandle().status().has_metadata;
			return hm.str();
		}
		inline std::string getHasIncomingString()
		{
			std::stringstream hi;
			hi << getHandle().status().has_incoming;
			return hi.str();
		}
		inline std::string getInfoHashString()
		{
			std::stringstream ih;
			ih << getHandle().status().info_hash;
			return ih.str();
		}
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

		void setSequentialDownload(bool seq);
		bool SequentialDownloadEnabled();
		std::vector<std::string> filenames();
	};
}
