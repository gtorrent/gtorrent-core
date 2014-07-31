#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include "libtorrent.hpp"
#include "Torrent.hpp"

namespace gt
{
	class Core
	{
	private:
		std::vector<std::shared_ptr<Torrent>> m_torrents;
		libtorrent::session m_session;
		bool m_running;
	public:
		Core(int argc = 0, char **argv = nullptr);

		inline vector<std::shared_ptr<Torrent>> &getTorrents()
		{
			return m_torrents;
		};

		static bool isMagnetLink(std::string const& link);

		int loadSession(std::string path);
		int saveSession(std::string path);
		shared_ptr<Torrent> addTorrent(std::string path, std::vector<char> *resumedata = nullptr);

		void removeTorrent(std::shared_ptr<Torrent>);

		inline bool isRunning() const
		{
			return m_running;
		};
		void shutdown();
		shared_ptr<Torrent> update();
		void setSessionParameters();
	};
}
