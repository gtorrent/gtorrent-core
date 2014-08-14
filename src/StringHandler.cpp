#define T_PPM 1000000.f

string getFileSizeString(boost::int64_t file_size)
{
	if (file_size <= 0)
	{
		return string();
	}

	ostringstream fss;
	fss << setprecision(3);

	if (file_size >= (1024 * 1024 * 1024))
	{
		fss << file_size / (double)(1024 * 1024 * 1024) << " GB";
	}
	else if (file_size >= (1024 * 1024))
	{
		fss << (file_size / (double)(1024 * 1024)) << " MB";
	}
	else if (file_size >= 1024)
	{
		fss << (file_size / (double)1024) << " KB";
	}
	else if (file_size > 0)
	{
		fss << file_size << " B";
	}
	return fss.str();
}

string getRateString(boost::int64_t file_rate)
{
	ostringstream frs;
	if (file_rate > 0)
	{
		frs << getFileSizeString(file_rate) << "/s";
	}
	return frs.str();
}

// format 0d 0h 0m 0s
string getTimeString( boost::int64_t time_s )
{
	if ( time_s <= 0 )
		return "∞";

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

// Returns a friendly string for the total size of files remaining to download in torrent
inline std::string getRemainingString()
{
	return getFileSizeString(getRemaining());
}

// Returns a friendly string for the current ratio
std::string getTotalRatioString();

// Returns a friendly string for the current time remaining
std::string getTimeRemainingString()
{
			return getTimeString(getTimeRemaining());
}

std::string getWantedDoneString()
{
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

    inline std::string getIpFilterAppliesString()
		{
			return to_string(getHandle().status().ip_filter_applies);
		}
		inline std::string getPausedString()
		{
			return to_string(getHandle().status().paused);
		}
		inline std::string getAutoManagedString()
		{
			return to_string(getHandle().status().auto_managed);
		}
		inline std::string getSequentialDownloadString()
		{
			return to_string(getHandle().status().sequential_download);
		}
		inline std::string getIsSeedingString()
		{
			return to_string(getHandle().status().is_seeding);
		}
		inline std::string getIsFinishedString()
		{
			return to_string(getHandle().status().is_finished);
		}
		inline bool hasMetadata()
		{
			return getHandle().status().has_metadata;
		}
		inline std::string getHasMetadataString()
		{
			return to_string(getHandle().status().has_metadata);
		}
		inline std::string getHasIncomingString()
		{
			return to_string(getHandle().status().has_incoming);
		}

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
string gt::Torrent::getStateString()
{
	ostringstream o;
	int precision = 1;

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
	o << fixed << setprecision(precision) << getTotalProgress() << '%';
	return o.str();

}

string gt::Torrent::getTotalRatioString()
{
	ostringstream ttr;
	ttr << fixed << setprecision(3) << getTotalRatio();
	return ttr.str();
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


		// Returns formatted active time as string
		inline std::string getActiveTimeString()
		{
			return getTimeString(getActiveTime());
		}

    		inline std::string getAllTimeDownloadString()
		{
			//size_type all_time_download;
			return getFileSizeString(getHandle().status().all_time_download);
		}

    		inline std::string getStorageModeString()
		{
			return to_string(getHandle().status().storage_mode);
		}

		inline std::string getProgressString()
		{
			return to_string(getHandle().status().progress);
		}
		inline std::string getProgressPpmString()
		{
			return to_string(getHandle().status().progress_ppm);
		}

		inline std::string getNumSeedsString()
		{
			return to_string(getHandle().status().num_seeds);
		}
		inline std::string getNumPeersString()
		{
			return to_string(getHandle().status().num_peers);
		}
		inline std::string getNumCompleteString()
		{
			return to_string(getHandle().status().num_complete);
		}
		inline std::string getNumIncompleteString()
		{
			return to_string(getHandle().status().num_incomplete);
		}
		inline std::string getListSeedsString()
		{
			return to_string(getHandle().status().list_seeds);
		}
		inline std::string getListPeersString()
		{
			return to_string(getHandle().status().list_peers);
		}
		inline std::string getConnectCandidatesString()
		{
			return to_string(getHandle().status().connect_candidates);
		}
		inline std::string getDistributedCopiesString()
		{
			return to_string(getHandle().status().distributed_copies);
		}

		inline std::string getNumUploadsString()
		{
			return to_string(getHandle().status().num_uploads);
		}
		inline std::string getNumConnectionsString()
		{
			return to_string(getHandle().status().num_connections);
		}
		inline std::string getSeedingTimeString()
		{
			return getTimeString(getHandle().status().seeding_time);
		}
		inline std::string getSeedRankString()
		{
			return to_string(getHandle().status().seed_rank);
		}
		inline std::string getLastScrapeString()
		{
			return getTimeString(getHandle().status().last_scrape);
		}
		inline std::string getSparseRegionsString()
		{
			return to_string(getHandle().status().sparse_regions);
		}
		inline std::int getPriorityString()
		{
			return to_string(getHandle().status().priority);
		}
    	inline std::string getStorageModeString()
		{
			return to_string(getHandle().status().storage_mode);
		}

			//Returns the info hash for this torrent
		inline std::sha1_hash getInfoHash()
		{
			std::string ih;
			ih << getHandle().status().info_hash;
			return ih.;
		}

//Returns the number of seconds this torrent has been seeding for
inline std::int getSeedingTime()
{
	return getTimeString(getHandle().status().seeding_time;
}
