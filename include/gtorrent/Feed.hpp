#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <libtorrent/rss.hpp>
#include <Torrent.hpp>
#include <Core.hpp>

namespace gt
{
	class Feed : public libtorrent::feed_handle
	{
		gt::Core *core;
		// this is used only when getFilteredItems is called without arguments
		// The idea is that you can filter based on "columns" determined by regexes
		// For example, consider a feed containing titles such as: [xXx] Series - XY [ZZZZp].ext
		// You can add a columns Episode matching XY with the following regex : /\-\ ..?\ \[/
		// or Quality: /\[.*p\]/
		// or something like that, I'm not really good at regexp
		// then you can add a filtering comparisons such as Episode > 3 or Quality = 720, Quality ! 720
		// the rhs can only be a constant
		// a column is always on the left hand side or the comparison, and will be converter to an integer type when the rhs is made only of digits
		// Filters can be written without whitespace, >, <, =, ! are used as delimiters. trailing or leading whitespace is ignored
		std::set<std::string> functions;

	public:
		// associates a filter name (column) to a regex
		// filtering functions
		std::map<std::string, std::string> filters;
		typedef std::function<void(std::shared_ptr<gt::Feed>)> feedCallback;
		typedef std::function<void(const libtorrent::feed_item&, std::shared_ptr<gt::Feed>)> itemCallback;
		void addFunction(std::string);
		void addItem(const libtorrent::feed_item &fi);
		std::function<void(int, std::shared_ptr<gt::Feed>)> onStateChanged;

		// Ordered in the important to less important callbacks
		feedCallback onUpdateFinished;
		feedCallback onUpdateErrored;
		itemCallback onNewItemAvailable;
		feedCallback onUpdateStarted;
		std::set<std::string> &getFunctions();
		Feed(const libtorrent::feed_handle &fe, gt::Core *Core, feedCallback finishedCallback = feedCallback(), feedCallback erroredCallback = feedCallback(), itemCallback newItemCallback = itemCallback(), feedCallback startedCallback = feedCallback());

		std::vector<libtorrent::feed_item> getFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<libtorrent::feed_item> getFilteredItems();
		// jesus christ looks at these return types
		std::vector<std::shared_ptr<gt::Torrent>> addFilteredItems(std::function<bool(std::string)> filterFun);
		std::vector<std::shared_ptr<gt::Torrent>> addFilteredItems();
		bool operator==(const libtorrent::feed_handle& rhs) { return rhs.settings().url == settings().url; }
	};
}
