#ifndef TORRENT_HPP
#define TORRENT_HPP
#include "libtorrent.hpp"
#include "Event.hpp"
#include <memory>

class Torrent
{
private:
	libtorrent::add_torrent_params m_torrent_params;
	libtorrent::torrent_handle m_handle;
	std::string m_path;
public:
	Torrent(std::string path);

	bool pollEvent(gt::Event &event);

	/* Getters */
	libtorrent::add_torrent_params getTorrentParams();
	std::string getPath();
	
	/* Returns percentage of all files downloading */
	unsigned int getTotalProgress();

	/* Setters */
	void setHandle(libtorrent::torrent_handle &h);
};

#endif