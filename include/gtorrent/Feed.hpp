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
		std::function<void(int, std::shared_ptr<gt::Feed>)> onStateChanged;
		std::function<void(std::shared_ptr<gt::Feed>)> onUpdateStarted;
		std::function<void(std::shared_ptr<gt::Feed>)> onUpdateFinished;
		std::function<void(std::shared_ptr<gt::Feed>)> onUpdateErrored;
		std::function<void(libtorrent::feed_item, const Feed &)> onNewItemAvailable;
		Feed(std::string Url, gt::Core *Core);
		std::vector<libtorrent::feed_item> getFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<gt::Torrent> addFilteredItems(std::function<bool(std::string)> filterFun);
		bool operator==(const libtorrent::feed_handle& rhs) { return rhs.settings().url == settings().url; }
	};
}
