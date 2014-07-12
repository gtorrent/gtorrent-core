#ifndef TORRENTENGINE_HPP
#define TORRENTENGINE_HPP
#include "libtorrent.hpp"
#include "Torrent.hpp"
#include <memory>
#include <vector>

class TorrentEngine;	typedef std::shared_ptr<TorrentEngine> te_ptr;
typedef std::vector<std::shared_ptr<Torrent> > te_ptrc;

class TorrentEngine
{
private:
	libtorrent::session m_session;
	te_ptrc m_torrents;
public:
	TorrentEngine();

	te_ptrc &getTorrents();

	t_ptr addTorrent(std::string path);
	void queue();
};
#endif