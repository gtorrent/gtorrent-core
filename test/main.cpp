#include <gtorrent/libtorrent.hpp>
#include <gtorrent/Core.hpp>
#include <gtorrent/Event.hpp>
#include <gtorrent/Log.hpp>
#include <gtorrent/Torrent.hpp>

int main(int argc, char *argv[]) {
	printf("gTest.exe started\n");
	gt::Log::Debug("Testing logging");
    gt::Log::Debug("Starting core...");
    shared_ptr<gt::Core> m_core(new gt::Core);
    gt::Log::Debug("Testing if core is running");
    gt::Log::Debug("%d",m_core->isRunning());
    gt::Log::Debug("Shutting down core...");
    m_core->shutdown();
	return 0;
}

