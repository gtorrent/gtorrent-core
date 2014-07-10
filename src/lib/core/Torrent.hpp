#ifndef TORRENT_HPP
#define TORRENT_HPP
#include "libtorrent.hpp"
#include <memory>

class Torrent
{
private:
	libtorrent::add_torrent_params m_torrent_params;
	libtorrent::torrent_handle m_handle;
public:
	Torrent(std::string path);

	bool pollEvent();

	libtorrent::add_torrent_params getTorrentParams();

	void setHandle(libtorrent::torrent_handle &h);
};

#endif