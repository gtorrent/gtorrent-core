#ifndef TORRENTENGINE_HPP
#define TORRENTENGINE_HPP
#include "libtorrent.hpp"
#include "Torrent.hpp"
#include <memory>
#include <vector>

class TorrentEngine;	typedef std::unique_ptr<TorrentEngine> te_ptr;

class TorrentEngine
{
private:
	libtorrent::session m_session;
	std::vector<std::shared_ptr<Torrent> > m_torrents;
public:
	TorrentEngine();

	void addTorrent(std::string path);
	void queue();
};
#endif