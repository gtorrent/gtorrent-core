#ifndef TORRENT_HPP
#define TORRENT_HPP
#include "libtorrent.hpp"
#include <memory>

class Torrent
{
private:
	libtorrent::add_torrent_params m_torrent_params;
public:
	Torrent(std::string path);

	libtorrent::add_torrent_params getTorrentParams();
};

#endif