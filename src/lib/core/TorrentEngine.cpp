#include "TorrentEngine.hpp"

TorrentEngine::TorrentEngine()
{
	m_session.listen_on(std::make_pair(6881, 6889));
}

void TorrentEngine::addTorrent(std::string path)
{
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

	printf("Total Size:\t%i\n\n", fs.total_size());

	printf("Downloading data from \"%s\"...\n", path.c_str());
}

void TorrentEngine::queue()
{
	auto iter = std::begin(m_torrents);

	while (iter != std::end(m_torrents))
	{
		auto &t = **iter;

		gt::Event event;

		if (t.pollEvent(event))
		{
			switch (event.type)
			{
				case gt::Event::DownloadCompleted:
					printf("Done!\n");
					iter = m_torrents.erase(iter);
				break;
			}
		}
		else
		{
			++iter;
		}
	}
}