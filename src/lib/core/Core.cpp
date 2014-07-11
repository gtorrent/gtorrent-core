#include "Core.hpp"
#include <stdio.h>

GTorrent_Core::GTorrent_Core() :
	m_running(true)
{
	m_engine = te_ptr(new TorrentEngine());
}

te_ptr GTorrent_Core::getEngine()
{
	return m_engine;
}

bool GTorrent_Core::isRunning()
{
	return m_running;
}

void GTorrent_Core::shutdown()
{
	m_running = false;
}

void GTorrent_Core::update()
{
	m_engine->queue();
}