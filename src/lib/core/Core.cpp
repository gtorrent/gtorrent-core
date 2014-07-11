#include "Core.hpp"
#include <stdio.h>

GTorrent_Core::GTorrent_Core()
{
	m_engine = te_ptr(new TorrentEngine());
	m_engine->addTorrent("dsl.torrent");
}

te_ptr GTorrent_Core::getEngine()
{
	return m_engine;
}

bool GTorrent_Core::isRunning()
{
	return true;
}

void GTorrent_Core::update()
{
	m_engine->queue();
}