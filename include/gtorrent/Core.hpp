#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libtorrent/session.hpp>

namespace gt
{
	// TODO move to own file
	class TorrentGroup
	{
	public:
		// TODO Make proper get functions
		std::vector<std::shared_ptr<Torrent>> m_torrents_all;
		std::vector<std::shared_ptr<Torrent>> m_torrents_downloading;
		std::vector<std::shared_ptr<Torrent>> m_torrents_seeding;
		std::vector<std::shared_ptr<Torrent>> m_torrents_checking;
		std::vector<std::shared_ptr<Torrent>> m_torrents_finished;
		std::vector<std::shared_ptr<Torrent>> m_torrents_stopped;
		std::vector<std::shared_ptr<Torrent>> m_torrents_paused;

		TorrentGroup();

		int update();

		void add(std::shared_ptr<gt::Torrent>);

		void remove(std::shared_ptr<gt::Torrent>);

		int updateStatus();
	};

	class Core
	{
		friend class FeedGroup;

		TorrentGroup m_torrents;
		std::deque<std::shared_ptr<gt::Torrent>> m_pendingTorrents; //Contains torrent that have been added to the session but that haven't been reported to the UI
		// TODO add a TorrentGroup vector once group functionality has been tested with m_torrents

	public:
		std::vector<std::shared_ptr<gt::FeedGroup>> m_feeds;
		std::vector<std::shared_ptr<gt::Feed>> m_feedhandles;
		libtorrent::session m_session;
		bool m_running;

		Core(int argc = 0, char **argv = nullptr);

		static bool isLink(std::string const &link); // Does this really need to be in fucking gt::Core?

		std::vector<std::shared_ptr<gt::Torrent>> &getTorrents();

		TorrentGroup *getAllTorrents();

		std::vector<TorrentGroup> getAllGroups(); // TODO implement
		std::shared_ptr<gt::Torrent> addTorrent(std::string path, std::vector<char> *resumedata = nullptr);

		std::shared_ptr<gt::Feed> addFeed(std::string Url);

		std::shared_ptr<gt::FeedGroup> addFeedGroup(std::string Name);

		int loadSession(std::string path);

		int saveSession(std::string path);

		void setSessionParameters();

		void removeFeed(std::shared_ptr<gt::Feed>);

		void removeTorrent(std::shared_ptr<gt::Torrent>);

		bool isRunning() const;

		void shutdown();

		std::shared_ptr<gt::Torrent> update();
	};
}
