#include "TorrentEngine.hpp"

TorrentEngine::TorrentEngine()
{
	m_session.listen_on(std::make_pair(6881, 6889));
}

void TorrentEngine::addTorrent(std::string path)
{
	printf("Downloading data from \"%s\":\n\n", path.c_str());

	std::shared_ptr<Torrent> t = std::shared_ptr<Torrent>(new Torrent(path));
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams());

	libtorrent::torrent_info info = h.get_torrent_info();
	libtorrent::file_storage fs = info.files();

	t->setHandle(h);
	m_torrents.push_back(t);

	/* An Example For Fetching All Files From Torrent: */

	unsigned int numFiles = fs.num_files();

	printf("Total Files: %i\n", fs.num_files());
	printf("=========================================\n");

	for (unsigned int i = 0; i < numFiles; ++i)
	{
		libtorrent::file_entry e = fs.at(i);

		printf("Name:\t\t%s\n", e.path.c_str());
		printf("Size:\t\t%i\n", e.size);
		printf("-----------------------------------------\n");
	}

	printf("Total Size:\t%i\n", fs.total_size());
}

void TorrentEngine::queue()
{
	auto iter = std::begin(m_torrents);

	while (iter != std::end(m_torrents))
	{
		auto &t = **iter;

		/* TODO: Check for "finish" event and move out of downloading loop instead */

		if (t.pollEvent())
		{
			iter = m_torrents.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}