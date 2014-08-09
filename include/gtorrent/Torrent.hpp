#pragma once

#include <ctime>

#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"

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

		std::string YesNo(bool input) { return input ? "Yes" : "No"; }

		// Getters
		inline libtorrent::add_torrent_params getTorrentParams() { return m_torrent_params; }
		inline libtorrent::torrent_handle& getHandle() { return m_handle; }
		inline std::string& getPath() { return m_path; }
		inline boost::int64_t getActiveTime() { return m_handle.status().active_time; }

		inline boost::int64_t getEta() { return (getDownloadRate() <= 0) ? -1 : (getWanted() / getDownloadRate()); }

		inline std::string getEtaString() { return getTimeString(getEta()); }

		std::vector<bool> getPieces();

		inline float getTotalProgress() { return ((float) getHandle().status().progress_ppm / 1000000.0f) * 100.0f; }
		inline unsigned int getUploadRate() { return (isPaused() ? 0 : getHandle().status().upload_rate); }
		inline unsigned int getDownloadRate() { return (isPaused() ? 0 : getHandle().status().download_rate); }
		inline unsigned int getPPMProgress() { return getHandle().status().progress_ppm; }
		inline unsigned int getTotalSeeders() { return getHandle().status().num_seeds; }
		inline unsigned int getTotalPeers() { return getHandle().status().num_peers; }
		inline unsigned int getTotalLeechers() { return getTotalPeers() - getTotalSeeders(); }
		inline boost::int64_t getTotalUploaded() { return getHandle().status().total_upload; }
		inline boost::int64_t getTotalDownloaded() { return getHandle().status().total_download; }
		inline boost::int64_t getSize() { return getHandle().status().total_wanted; }
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getWanted() { return getSize(); }
		// TODO: Remove this, duplicate of getSize()
		inline boost::int64_t getTorrentSize() { return getSize(); }
		inline boost::int64_t getTimeRemaining() { return (getDownloadRate() > 0) ? getTorrentSize() / getDownloadRate() : 0; }
		float getTotalRatio();
		inline libtorrent::torrent_status::state_t getState() { return m_handle.status().state; }
		inline std::string getCurrentTrackerURL() { return m_handle.status().current_tracker; }
		void torrentForceRecheck();
		std::string getStateString();
		inline std::string getUploadRateString() { return getRateString(getUploadRate()); }
		inline std::string getDownloadRateString() { return getRateString(getDownloadRate()); }
		inline std::string getTotalUploadedString() { return getFileSizeString(getTotalUploaded()); }
		inline std::string getTotalDownloadedString() { return getFileSizeString(getTotalDownloaded()); }
		inline std::string getSizeString() { return getFileSizeString(getSize()); }
		inline boost::int64_t getRemaining() { return getSize() - getTotalDownloaded(); }
		inline std::string getRemainingString() { return getFileSizeString(getRemaining()); }


		std::string getTotalRatioString();
		inline std::string getTimeRemainingString() { return getTimeString(getTimeRemaining()); }
		inline bool isPaused() { return getHandle().status().paused; }

		// Setters
		inline void setHandle(libtorrent::torrent_handle &h) { m_handle = h; }
		void setSavePath(std::string savepath);
		void setPaused(bool isPaused);
		inline void resume() { setPaused(false); }
		inline void pause() { setPaused(true); }
		void setSequentialDownload(bool seq);
		bool SequentialDownloadEnabled();

		std::vector<std::string> filenames();

		inline std::string getErrorString() { return getHandle().status().error; }
		inline std::string getSavePath() { return getHandle().status().save_path; }
		inline std::string getSavePathString() { return getHandle().status().save_path; }
		inline std::string getName() { return getHandle().status().name; }
		inline std::string getTorrentFileNameString()
		{
			std::stringstream tfn;
			tfn << "torrent-filename.torrent";
			return tfn.str();
		}
		inline std::string getNextAnnounceString()
		{
			std::stringstream na;
			na << getHandle().status().next_announce;
			return na.str();
		}
		inline std::string getAnnounceIntervalString() { return to_simple_string(getHandle().status().announce_interval); }
		inline std::string getCurrentTrackerString() { return getHandle().status().current_tracker; }
		inline std::string getTotalPayloadDownloadString() { return getFileSizeString(getHandle().status().total_payload_download); }
		inline std::string getTotalPayloadUploadString() { return getFileSizeString(getHandle().status().total_payload_upload); }
		inline std::string getFailedBytesString() { return getFileSizeString(getHandle().status().total_failed_bytes); }
		inline std::string getRedundantBytesString() { return getFileSizeString(getHandle().status().total_redundant_bytes); }
		inline std::string getTotalDoneString() { return getFileSizeString(getHandle().status().total_done); }
		inline std::string getWantedDoneString() { return getFileSizeString(getHandle().status().total_wanted_done); }
		inline std::string getTotalWantedString() { return getFileSizeString(getHandle().status().total_wanted); }
		inline std::string getAllTimeUploadString() { return getFileSizeString(getHandle().status().all_time_upload); }
		inline std::string getAllTimeDownloadString() { return getFileSizeString(getHandle().status().all_time_download); }

		inline std::string getAddedTimeString() { return getTimeString(getHandle().status().added_time); }
		inline std::string getCompletedTimeString() { return getTimeString(getHandle().status().completed_time); }
		inline std::string getLastSeenCompleteString() { return getTimeString(getHandle().status().last_seen_complete); }

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
		inline std::string getQueuePositionString()
		{
			std::stringstream qp;
			qp << getHandle().status().queue_position;
			return qp.str();
		}
		inline std::string getDownloadPayloadRateString()
		{
			std::stringstream dpr;
			dpr << getHandle().status().download_payload_rate;
			return dpr.str();
		}
		inline std::string getUploadPayloadRateString()
		{
			std::stringstream upr;
			upr << getHandle().status().upload_payload_rate;
			return upr.str();
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
		inline std::string getNumPiecesString()
		{
			std::stringstream np;
			np << getHandle().status().num_pieces;
			return np.str();
		}
		inline std::string getDistributedFullCopiesString()
		{
			std::stringstream dfc;
			dfc << getHandle().status().distributed_full_copies;
			return dfc.str();
		}
		inline std::string getDistributedFractionString()
		{
			std::stringstream df;
			df << getHandle().status().distributed_fraction;
			return df.str();
		}
		inline std::string getDistributedCopiesString()
		{
			std::stringstream dc;
			dc << getHandle().status().distributed_copies;
			return dc.str();
		}
		inline std::string getBlockSizeString() { return getFileSizeString(getHandle().status().block_size); }
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
		inline std::string getUploadsLimitString()
		{
			std::stringstream ul;
			ul << getHandle().status().uploads_limit;
			return ul.str();
		}
		inline std::string getConnectionsLimitString()
		{
			std::stringstream cl;
			cl << getHandle().status().connections_limit;
			return cl.str();
		}
		inline std::string getUpBandwidthQueueString()
		{
			std::stringstream ubq;
			ubq << getHandle().status().up_bandwidth_queue;
			return ubq.str();
		}
		inline std::string getDownBandwidthQueueString()
		{
			std::stringstream dbq;
			dbq << getHandle().status().down_bandwidth_queue;
			return dbq.str();
		}

		inline std::string getTimeSinceUploadString() { return getTimeString(getHandle().status().time_since_upload); }
		inline std::string getTimeSinceDownloadString() { return getTimeString(getHandle().status().time_since_download); }
		inline std::string getActiveTimeString() { return getTimeString(getActiveTime()); }
		inline std::string getFinishedTimeString() { return getTimeString(getHandle().status().finished_time); }
		inline std::string getSeedingTimeString() { return getTimeString(getHandle().status().seeding_time); }
		inline std::string getSeedRankString()
		{
			std::stringstream sr;
			sr << getHandle().status().seed_rank;
			return sr.str();
		}
		inline std::string getLastScrapeString() { return getTimeString(getHandle().status().last_scrape); }
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
		inline std::string getNeedSaveResumeString() { return YesNo(getHandle().status().need_save_resume); }
		inline std::string getIpFilterAppliesString() { return YesNo(getHandle().status().ip_filter_applies); }
		inline std::string getUploadModeString() { return YesNo(getHandle().status().upload_mode); }
		inline std::string getShareModeString() { return YesNo(getHandle().status().share_mode); }
		inline std::string getSuperSeedingString() { return YesNo(getHandle().status().super_seeding); }
		inline std::string getPausedString() { return YesNo(getHandle().status().paused); }
		inline std::string getAutoManagedString() { return YesNo(getHandle().status().auto_managed); }
		inline std::string getSequentialDownloadString() { return YesNo(getHandle().status().sequential_download); }
		inline std::string getIsSeedingString() { return YesNo(getHandle().status().is_seeding); }
		inline std::string getIsFinishedString() { return YesNo(getHandle().status().is_finished); }
		inline bool hasMetadata() { return getHandle().status().has_metadata; }
		inline std::string getHasMetadataString() { return YesNo(getHandle().status().has_metadata); }
		inline std::string getHasIncomingString() { return YesNo(getHandle().status().has_incoming); }
		inline std::string getSeedModeString() { return YesNo(getHandle().status().seed_mode); }
		inline std::string getMovingStorageString() { return YesNo(getHandle().status().moving_storage); }
		inline std::string getInfoHashString()
		{
			std::stringstream ih;
			ih << getHandle().status().info_hash;
			return ih.str();
		}
		inline boost::intrusive_ptr<libtorrent::torrent_info const> getInfo() { return getHandle().torrent_file(); }
	};
}
