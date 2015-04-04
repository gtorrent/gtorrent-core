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

	class Torrent : public libtorrent::torrent_handle
	{
	private:
		unsigned int m_id;
		libtorrent::add_torrent_params m_torrent_params;
//		__attribute__((deprecated("use this directly")))
		libtorrent::torrent_handle m_handle; // this will be removed when -gtk will stop relying on getHandle()
		std::string m_path;

	public:
		Torrent(libtorrent::torrent_handle h) : libtorrent::torrent_handle(h) {  };
		Torrent(std::string path);
		std::function<void(int, std::shared_ptr<Torrent>)> onStateChanged; // why does this remind me of kirby <('_')>

		bool pollEvent(gt::Event &event);
		void defaultCallback(int, std::shared_ptr<Torrent>);
		/* Think twice next time before mixing const correctness with inline */
		// Getters
		libtorrent::add_torrent_params getTorrentParams();

		std::string& getPath();

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

		// Returns the ratio (uploaded/downloaded) for this torrent
		float getTotalRatio();

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

		// Returns a friendly string for the total size of files remaining to download in torrent
		std::string getTextRemaining();

		// Returns a friendly string for the current ratio
		std::string getTextTotalRatio();

		// Returns a friendly string for the current time remaining
		std::string getTextTimeRemaining();

		bool isPaused();

		// Setters
		void setSavePath(std::string savepath);

		void setPaused(bool isPaused);

		std::string getSavePath();

		std::string getFormattedHash();

		//libtorrent::add_torrent_params.ti is an intrusive_ptr in 1.0 and a shared_ptr in svn.
		//Using decltype allows us to make it compatible with both versions.
		typedef decltype(boost::const_pointer_cast<const libtorrent::torrent_info>(m_torrent_params.ti)) getInfoReturnType;

		bool SequentialDownloadEnabled();
		std::vector<std::string> filenames();

		std::vector<Peer> getPeers();
	};
}
