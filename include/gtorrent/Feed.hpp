#pragma once

#include <string>
#include <memory>
#include <vector>
#include <libtorrent/rss.hpp>
#include <Torrent.hpp>
#include <Core.hpp>

namespace gt
{
	class Feed : public libtorrent::feed_handle
	{
		gt::Core *core;
	public:
		std::function<void(const Feed &)> onUpdateStarted;
		std::function<void(const Feed &)> onUpdateFinished;
		std::function<void(const Feed &)> onUpdateErrored;
		Feed(std::string Url, gt::Core *Core);
		std::vector<libtorrent::feed_item> getFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<gt::Torrent> addFilteredItems(std::function<bool(std::string)> filterFun);
	};
}
