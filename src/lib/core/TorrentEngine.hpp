#ifndef TORRENTENGINE_HPP
#define TORRENTENGINE_HPP
#include "libtorrent.hpp"
#include <memory>

class TorrentEngine;	typedef std::unique_ptr<TorrentEngine> te_ptr;

class TorrentEngine
{
private:
	libtorrent::session m_session;
public:
	TorrentEngine();

	void addTorrent(std::string path);
	void update();
};
#endif