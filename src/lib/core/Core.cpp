#include "Core.hpp"
#include <stdio.h>

GTorrent_Core::GTorrent_Core()
{
	m_engine = te_ptr(new TorrentEngine());
	m_engine->addTorrent("dsl.torrent");
	
	while (true); /* Need to Poll For Events */
}