#include <Feed.hpp>
#include <Log.hpp>

using namespace std;

// load default feed settings but change them quickly later
gt::Feed::Feed(const libtorrent::feed_handle &fe, gt::Core *Core, feedCallback finishedCallback, feedCallback erroredCallback, itemCallback newItemCallback, feedCallback startedCallback) :
	feed_handle(fe),
	core(Core),
	onUpdateFinished(finishedCallback),
	onUpdateErrored(erroredCallback),
	onNewItemAvailable(newItemCallback),
	onUpdateStarted(startedCallback)
{
	update_feed();
}

/*void addItem(libtorrent::feed_item &fi)
{
	
}
*/

vector<libtorrent::feed_item> gt::Feed::getFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<libtorrent::feed_item> ret;
	auto fs = get_feed_status();
	for(auto item : fs.items)
		if(filterFun(item.title))
			ret.push_back(item);
	return ret;
}

vector<gt::Torrent> gt::Feed::addFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<gt::Torrent> ret;
	for(auto item : getFilteredItems(filterFun))
		core->addTorrent(item.url);
	return ret;	
}
