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
    gt::Log::Debug("%s",m_core->isRunning() ? "Core is running" : "Core is not running");
    gt::Log::Debug("Adding Torrent...");
    shared_ptr<Torrent> m_torrent = m_core->addTorrent("debian-7.6.0-amd64-netinst.iso.torrent");
    gt::Log::Debug("Shutting down core...");
    m_core->shutdown();
	return 0;
}
