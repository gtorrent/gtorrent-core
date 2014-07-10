#include "Core.hpp"
#include <stdio.h>

GTorrent_Core::GTorrent_Core()
{
	m_engine = te_ptr(new TorrentEngine());
	m_engine->addTorrent("dsl.torrent");
	
	/* Need Actual Event Polling */

	while (true)
	{
		m_engine->queue();
	}
}