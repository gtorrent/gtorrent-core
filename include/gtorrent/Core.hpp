#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libtorrent/session.hpp>

namespace gt
{
	class Torrent;
	class Core
	{
	private:
		std::vector<std::shared_ptr<gt::Torrent>> m_torrents;
		libtorrent::session m_session;
		bool m_running;
	public:
		Core(int argc = 0, char **argv = nullptr);

		inline std::vector<std::shared_ptr<Torrent>> &getTorrents()
		{
			return m_torrents;
		};

		static bool isMagnetLink(std::string const& link);

		int loadSession(std::string path);
		int saveSession(std::string path);
		std::shared_ptr<gt::Torrent> addTorrent(std::string path, std::vector<char> *resumedata = nullptr);

		void removeTorrent(std::shared_ptr<gt::Torrent>);

		inline bool isRunning() const
		{
			return m_running;
		};
		void shutdown();
		std::shared_ptr<gt::Torrent> update();
		void setSessionParameters();
	};
}
