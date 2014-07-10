#ifndef CORE_HPP
#define CORE_HPP
#include "TorrentEngine.hpp"
#include <memory>

class GTorrent_Core;	typedef std::shared_ptr<GTorrent_Core> core_ptr;

class GTorrent_Core
{
private:
	te_ptr m_engine;
public:
	GTorrent_Core();
};

#endif