#ifndef TORRENT_HPP
#define TORRENT_HPP
#include "libtorrent.hpp"
#include "Event.hpp"
#include <memory>

class Torrent;	typedef std::shared_ptr<Torrent> t_ptr;

class Torrent
{
private:
	unsigned int m_id;
	libtorrent::add_torrent_params m_torrent_params;
	libtorrent::torrent_handle m_handle;
	std::string m_path;
	
public:
	Torrent(std::string path);

	bool pollEvent(gt::Event &event);

	/* Getters */
	libtorrent::add_torrent_params getTorrentParams();
	libtorrent::torrent_handle &getHandle();
	std::string getPath();
	
	/* Returns percentage of all files downloading */
	float getTotalProgress();

	/* Returns the progress in PPM of all files downloading */
	unsigned int getPPMProgress();

	/* Returns the current number of seeders attached to the file */
	unsigned int getTotalSeeders();

	/* Returns the current number of peers attached to the file */
	unsigned int getTotalPeers();

	/* Setters */
	void setHandle(libtorrent::torrent_handle &h);
};

#endif