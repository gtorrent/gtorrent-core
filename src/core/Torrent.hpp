#pragma once

#include "libtorrent.hpp"
#include "Event.hpp"
#include <iomanip>
#include <sstream>

class Torrent
{
private:
	unsigned int m_id;
	libtorrent::add_torrent_params m_torrent_params;
	libtorrent::torrent_handle m_handle;
	string m_path;
	
public:
	Torrent(string path);
	
	bool pollEvent(gt::Event &event);

	// Getters
	libtorrent::add_torrent_params getTorrentParams();
	libtorrent::torrent_handle &getHandle();
	string getPath();
	
	// Returns percentage of all files downloading
	float getTotalProgress();

	// Returns the current download rate of the torrent
	unsigned int getDownloadRate();

	// Returns the progress in PPM of all files downloading in this torrent
	unsigned int getPPMProgress();

	// Returns the current number of seeders attached to the file
	unsigned int getTotalSeeders();

	// Returns the current number of peers attached to the file
	unsigned int getTotalPeers();

	// Returns the current number of leechers attached to the file
	unsigned int getTotalLeechers();

	// Returns the current torrent state (downloading, queueing, seeding, etc)
	libtorrent::torrent_status::state_t getState();

	// Returns a friendly string for the torrent state
	string getTextState();

	// Returns a friendly string for the current download rate
	string getTextDownloadRate();

	// Setters
	void setHandle(libtorrent::torrent_handle &h);
	void setSavePath(string savepath);
};

