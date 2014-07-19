#pragma once

#define SECOND 60 
#define HOUR 60 * 60

#include "libtorrent.hpp"
#include "Event.hpp"
#include <iomanip>
#include <sstream>
#include <fstream>

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

	// Returns number of seconds the torrent has been active
	boost::int64_t getActive();

	// Returns formatted active time as string
	string getTextActive();

	// Returns number of seconds eta for the torrent
	boost::int64_t getEta();

	// Returns formatted eta as string
	string getTextEta();

	// Returns percentage of all files downloading
	float getTotalProgress();

	// Returns the current upload rate of the torrent
	unsigned int getUploadRate();

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

	// Returns the current amount of data uploaded for this torrent
	boost::int64_t getTotalUploaded();

	// Returns the current amount of data downloaded for this torrent
	boost::int64_t getTotalDownloaded();

<<<<<<< HEAD
	// Returns the total size of files in this torrent
        boost::int64_t getSize();

	// Returns the total size of wanted files in this torrent
        boost::int64_t getWanted();
=======
	//Returns the size of the torrent
	boost::int64_t getTorrentSize();

	//Returns the elapsed time remaining in seconds
	boost::int64_t timeRemaining();
>>>>>>> d2d0c966c66c343b38c8f3a6c8a8cb508a03150b

	// Returns the ratio (uploaded/downloaded) for this torrent
	float getTotalRatio();

	// Returns the current torrent state (downloading, queueing, seeding, etc)
	libtorrent::torrent_status::state_t getState();

	// Returns a friendly string for the torrent state
	string getTextState();

	// Returns a friendly string for the current upload rate
	string getTextUploadRate();

	// Returns a friendly string for the current download rate
	string getTextDownloadRate();

	// Returns a friendly string for the current upload total
	string getTextTotalUploaded();

	// Returns a friendly string for the current download total
	string getTextTotalDownloaded();
	
	// Returns a friendly string for the total size of files in torrent
        string getTextSize();

	// Returns a the total size of files remaining to download in torrent
        boost::int64_t getRemaining();

	// Returns a friendly string for the total size of files remaining to download in torrent
        string getTextRemaining();

	// Returns a friendly string for the current ratio
	string getTextTotalRatio();

	// Returns a friendly string for the current time remaining
	string getTextTimeRemaining();

	// Setters
	void setHandle(libtorrent::torrent_handle &h);
	void setSavePath(string savepath);
};

