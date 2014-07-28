#pragma once

using namespace std;

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
		vector<shared_ptr<Torrent>> m_torrents;
		libtorrent::session m_session;
		bool m_running;
	public:
		Core();

		inline vector<shared_ptr<Torrent>> &getTorrents()
		{
			return m_torrents;
		};

		static bool isMagnetLink(string const& link);

		int loadSession(string path);
		int saveSession(string path);
		shared_ptr<Torrent> addTorrent(string path, vector<char> *resumedata = nullptr);
		shared_ptr<Torrent> addRss(string rssurl);

		void removeTorrent(shared_ptr<Torrent>);

		// Obtains from platform.
		string getDefaultSavePath();

		inline bool isRunning() const
		{
			return m_running;
		};
		void shutdown();
		void update();
	};
}
