#ifndef CORE_HPP
#define CORE_HPP
#include "TorrentEngine.hpp"
#include <memory>

class GTorrent_Core;	typedef std::shared_ptr<GTorrent_Core> core_ptr;

class GTorrent_Core
{
private:
	te_ptr m_engine;
	bool m_running;
public:
	GTorrent_Core();

	bool isRunning();
	void shutdown();
	void update();

	/* Getters */
	te_ptr getEngine();

	/* Useful function for determining if passed url is a magnet link */
	static bool isMagnetLink(std::string url);
};

#endif