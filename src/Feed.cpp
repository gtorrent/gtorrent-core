#include <Feed.hpp>
#include <Log.hpp>
#include <regex>

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

void gt::Feed::addItem(const libtorrent::feed_item &fi)
{
	gt::Log::Debug("Adding " + fi.title + "\t :^)");
}

vector<libtorrent::feed_item> gt::Feed::getFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<libtorrent::feed_item> ret;
	auto fs = get_feed_status();
	for(auto item : fs.items)
		if(filterFun(item.title))
			ret.push_back(item);
	return ret;
}

vector<libtorrent::feed_item> gt::Feed::getFilteredItems()
{
	vector<libtorrent::feed_item> ret;
	auto fs = get_feed_status();

	if(filters.empty()) return fs.items;

	std::function<bool(string&, string&)> bcomp = [](string &a, string &b)
		{
			if(a.empty() || b.empty()) return false;
			if(std::all_of(b.begin(), b.end(), [](char c){ return c >= '0' && c <= '9'; })) // if b is only digits
			{
				if(!std::all_of(a.begin(), a.end(), [](char c){ return c >= '0' && c <= '9'; }))
					return false; //a is not a number
				return stoi(a) < stoi(b);
			}// I don't know why would people compare rel between string for a filter, but there ya go
			return a < b;
		};

	std::function<bool(string, string)> comp[]
	{
		[bcomp](string a, string b) { return  bcomp(a, b); }, // <
		[bcomp](string a, string b) { return  bcomp(b, a); }, // >
		[bcomp](string a, string b) { return !bcomp(b, a) && !bcomp(a, b); }, // =
		[bcomp](string a, string b) { return !bcomp(b, a) || !bcomp(a, b); }  // !
	};

	for(auto function : functions)
	{
		string comparisonOps = "<>=! ";
		string columnName = string(function.begin(),
								   std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end())); // yup
		std::string dbgstr = "Parsed column name is " + columnName;
		gt::Log::Debug(dbgstr.c_str());
		std::regex reg(filters[columnName]);
		dbgstr = "Parsed regex is " + filters[columnName];
		gt::Log::Debug(dbgstr.c_str());
		std::smatch s;
		char op = *std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end() - 1);
		unsigned opIndex = comparisonOps.find(op);
		if(opIndex == std::string::npos) continue; // function is fucked
		dbgstr = "Parsed operation is " + op;
		gt::Log::Debug(dbgstr.c_str());

		std::function<bool(string, string)> compFun = comp[opIndex];
		string literal = function.substr(function.find_last_of(comparisonOps) + 1);
		dbgstr = "Parsed argument is " + literal;
		gt::Log::Debug(dbgstr.c_str());
		for(auto item : fs.items)
		{
			std::regex_search(item.title, s, reg); // only one group should be matched here;
			dbgstr = string("Matching ") + filters[columnName] + " in " + item.title;
			gt::Log::Debug(dbgstr.c_str());

			for(auto m : s)
			{
				dbgstr = string("Testing ") + string(m) + " " + op + " " + literal;
				gt::Log::Debug(dbgstr.c_str());

				if(compFun(m, literal))
				{
					dbgstr = "Title " + item.title + " matched function " + function;
					gt::Log::Debug(dbgstr.c_str());
					ret.push_back(item);
				}
			}
		}
	}
	return ret;
}

/* Do not use these two yet*/
vector<shared_ptr<gt::Torrent>> gt::Feed::addFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems(filterFun))
		ret.push_back(core->addTorrent(item.url));
	return ret;
}

vector<shared_ptr<gt::Torrent>> gt::Feed::addFilteredItems()
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems())
		ret.push_back(core->addTorrent(item.url));
	return ret;	
}

void gt::Feed::addFunction(std::string function)
{
	if(functions.find(function) == functions.end())
		return;
	functions.insert(function);
}

std::set<std::string> &gt::Feed::getFunctions()
{
	return functions;
}
