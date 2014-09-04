#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libtorrent/session.hpp>

namespace gt
{
	class Torrent;
	class Feed;
	class Core
	{
	private:
		std::vector<std::shared_ptr<gt::Torrent>> m_torrents;
		std::vector<std::shared_ptr<gt::Feed>> m_feeds;
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
	public:
		Core(int argc = 0, char **argv = nullptr);
		libtorrent::session m_session;
		bool m_running;

		std::vector<std::shared_ptr<gt::Torrent>> &getTorrents();

		static bool isLink(std::string const& link);

		int loadSession(std::string path);
		int saveSession(std::string path);
		std::shared_ptr<gt::Torrent> addTorrent(std::string path, std::vector<char> *resumedata = nullptr);
		std::shared_ptr<gt::Feed> addFeed(std::string Url);

		void removeFeed(std::shared_ptr<gt::Feed>);
		void removeTorrent(std::shared_ptr<gt::Torrent>);

		bool isRunning() const;
		void shutdown();
		std::shared_ptr<gt::Torrent> update();
		void setSessionParameters();
		statusList* getStatuses();
	};
}
