#include <gtorrent/libtorrent.hpp>
#include <gtorrent/Core.hpp>
#include <gtorrent/Event.hpp>
#include <gtorrent/Log.hpp>
#include <gtorrent/Torrent.hpp>

int main(int argc, char *argv[])
{
	if (argc <= 1)
		return 1;
	printf("gTest.exe started\n");
	gt::Log::Debug("Testing logging");
	gt::Log::Debug("Starting core...");
	std::shared_ptr<gt::Core> m_core(new gt::Core);
	gt::Log::Debug("Testing if core is running");
	gt::Log::Debug("%s", m_core->isRunning() ? "Core is running" : "Core is not running");

	std::shared_ptr<gt::Torrent> t = m_core->addTorrent(std::string(argv[1]));
	for (auto &l : t->getLabels())
		std::cout << l << std::endl;
	t->addLabel("label1");
	for (auto &l : t->getLabels())
		std::cout << l << std::endl;

	std::cout << "Has label1? " << t->hasLabel("label1") << std::endl;

	gt::Log::Debug("Shutting down core...");
	m_core->shutdown();
	return 0;
}

