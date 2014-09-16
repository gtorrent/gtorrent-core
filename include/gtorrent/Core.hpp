#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libtorrent/session.hpp>

namespace gt
{
	class Torrent;
	class Feed;
	class FeedGroup;
	class Core
	{
		friend class FeedGroup;
		std::vector<std::shared_ptr<gt::Torrent>> m_torrents;

		std::deque<std::shared_ptr<gt::Torrent>> m_pendingTorrents; //Contains torrent that have been added to the session but that haven't been reported to the UI
		struct statusList
		{
			std::vector<std::shared_ptr<Torrent>> downloading;
			std::vector<std::shared_ptr<Torrent>> seeding;
			std::vector<std::shared_ptr<Torrent>> checking;
			std::vector<std::shared_ptr<Torrent>> finished;
			std::vector<std::shared_ptr<Torrent>> stopped;
			std::vector<std::shared_ptr<Torrent>> paused;

			int update(std::vector<std::shared_ptr<Torrent>> *tl);
		} statuses;

	  std::map<std::string, std::vector<std::shared_ptr<gt::Torrent>>> trackers;
	  void updateTrackers();


		//libtorrent::session m_session;
		//bool m_running;
	public:
		std::vector<std::shared_ptr<gt::FeedGroup>> m_feeds;
		std::vector<std::shared_ptr<gt::Feed>> m_feedhandles;

		Core(int argc = 0, char **argv = nullptr);
		libtorrent::session m_session;
		bool m_running;

		std::vector<std::shared_ptr<gt::Torrent>> &getTorrents();

		static bool isLink(std::string const& link);

		int loadSession(std::string path);
		int saveSession(std::string path);
		std::shared_ptr<gt::Torrent>      addTorrent(std::string path, std::vector<char> *resumedata = nullptr);
		std::shared_ptr<gt::Feed>         addFeed(std::string Url);
		std::shared_ptr<gt::FeedGroup>    addFeedGroup(std::string Name);

		void removeFeed(std::shared_ptr<gt::Feed>);
		void removeTorrent(std::shared_ptr<gt::Torrent>);

		bool isRunning() const;
		void shutdown();
		std::shared_ptr<gt::Torrent> update();
		void setSessionParameters();
		statusList* getStatuses();
	  std::map<std::string, std::vector<std::shared_ptr<Torrent>>> *getTrackers();
	};
}
