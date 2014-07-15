#pragma once

#include "libtorrent.hpp"
#include "Torrent.hpp"
#include <memory>

class Core;

typedef std::shared_ptr<Core> core_ptr;
typedef vector<std::shared_ptr<Torrent> > tc_ptr;

class Core
{
private:
	libtorrent::session m_session;
	tc_ptr m_torrents;

	bool m_running;
public:
	Core();

	static bool isMagnetLink(string link);
	
	tc_ptr &getTorrents();
	t_ptr addTorrent(string path);
	bool isRunning();

	void shutdown();
	void update();
	
};

