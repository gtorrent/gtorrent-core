#pragma once

#include "libtorrent.hpp"
#include "Torrent.hpp"
#include <memory>
#include <string>

namespace gt
{
	class Core
	{
	private:
		vector<shared_ptr<Torrent> > m_torrents;
		libtorrent::session m_session;
		bool m_running;
	public:
		Core();

		static bool isMagnetLink(string const& link);

		static string getDefaultSavePath();
		vector<shared_ptr<Torrent> > &getTorrents();
		shared_ptr<Torrent> addTorrent(string path);

		bool isRunning();
		void shutdown();
		void update();
	};
}
