#include <FeedGroup.hpp>
#include <Log.hpp>
#include <Feed.hpp>
#include <regex>

using namespace std;

void gt::FeedGroup::addItem(const libtorrent::feed_item &fi)
{
	gt::Log::Debug("Adding " + fi.title + "\t :^)");
	std::string str(fi.url); // We remove some html escape codes
	unsigned i;
	if((i = str.find("amp;")) != str.size()) str.erase(i, 4);
	core->m_pendingTorrents.push_front(core->addTorrent(str));
}

vector<libtorrent::feed_item> gt::FeedGroup::getFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<libtorrent::feed_item> ret;
	for(auto f : m_feeds)
		for(auto item : f->get_feed_status().items)
			if(filterFun(item.title))
				ret.push_back(item);
	return ret;
}

vector<libtorrent::feed_item> gt::FeedGroup::getFilteredItems()
{
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

	vector<libtorrent::feed_item> ret;
	if(filters.empty()) 
		for(auto f : m_feeds)
			for(auto item : f->get_feed_status().items)
				ret.push_back(item);

	for(auto f : m_feeds)
	{
		auto fs = f->get_feed_status();
		for(auto function : functions)
		{
			string comparisonOps = "<>=! ";
			string columnName = string(function.begin(),
									   std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end())); // yup
			std::regex reg(filters[columnName]);

			std::smatch s;
			char op = *std::find_first_of(function.begin(), function.end(), comparisonOps.begin(), comparisonOps.end() - 1);
			unsigned opIndex = comparisonOps.find(op);
			if(opIndex == std::string::npos) continue; // function is fucked

			std::function<bool(string, string)> compFun = comp[opIndex];
			string literal = function.substr(function.find_last_of(comparisonOps) + 1);

			for(auto item : fs.items)
			{
				std::regex_search(item.title, s, reg); // only one group should be matched here;

				for(auto m : s)
					if(compFun(m, literal))
						ret.push_back(item);
			}
		}
	}
	return ret;
}

vector<shared_ptr<gt::Torrent>> gt::FeedGroup::addFilteredItems(std::function<bool(std::string)> filterFun)
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems(filterFun))
		ret.push_back(core->addTorrent(item.url));
	return ret;
}

vector<shared_ptr<gt::Torrent>> gt::FeedGroup::addFilteredItems()
{
	vector<shared_ptr<gt::Torrent>> ret;
	for(auto item : getFilteredItems())
		ret.push_back(core->addTorrent(item.url));
	return ret;	
}

void gt::FeedGroup::addFunction(std::string function)
{
	if(functions.find(function) == functions.end())
		return;
	functions.insert(function);
}

std::set<std::string> &gt::FeedGroup::getFunctions()
{
	return functions;
}


// TODO: For this to work as shown below, make the tokenizer skip leading/trailing whitespace around tokens []{},|
/*
 *  //compact
 *  [group name]
 *  { feedurl1, feedurl2 }
 *  { filter1|regex1, filter2|regex2 }
 *  { function1, function2 }
 *
 *
 *  [group name]
 *  {
 *      feedurl1,
 *      feedurl2
 *	}
 *  {
 *      filter1|regex1,
 *      filter2|regex2
 *  }
 *  {
 *      function1,
 *      function2
 *  }
 *
 *
 */

std::vector<std::shared_ptr<gt::FeedGroup>> gt::FeedGroup::fromString(std::string sData, gt::Core *m_core)
{
	std::vector<std::shared_ptr<gt::FeedGroup>> ret;
	auto i = sData.begin();

	// maybe it'd be better to separate opening tokens and closing tokens
	std::string tokens = "[]{},|";
	while(*i++ == '[')
	{
		auto feedg = std::make_shared<gt::FeedGroup>();
		//parse groupname
		auto j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		feedg->name = string(i, j);
		if(*j != ']')
		{
			gt::Log::Debug(string("Expected token ] instead of token ") + *j);
			return std::vector<std::shared_ptr<gt::FeedGroup>>();
		}
		i = std::find_first_of(j + 1, sData.end(), tokens.begin(), tokens.end());;

		//parse feed urls
		j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		if(*j != '{')
		{
			gt::Log::Debug(std::string("Expected token { instead of token ") + *j);
			return std::vector<std::shared_ptr<gt::FeedGroup>>();
		}
		i = std::find_first_of(j + 1, sData.end(), tokens.begin(), tokens.end());;


		// can maybe convert to following to do{}while();
		std::string feedurl;
		j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		feedurl = std::string(i, j);
		auto feed = m_core->addFeed(feedurl);
		feed->owners.push_back(feedg);
		feedg->m_feeds.push_back(feed);
		
		j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		while(*j == ',')
		{
			i = std::find_first_of(j + 1, sData.end(), tokens.begin(), tokens.end());;
			j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
			feedurl = std::string(i, j);
			auto feed = m_core->addFeed(feedurl);
			feed->owners.push_back(feedg);
			feedg->m_feeds.push_back(feed);
			i = std::find_first_of(j + 1, sData.end(), tokens.begin(), tokens.end());;
			j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		}

		if(*j != '}')
		{
			gt::Log::Debug(std::string("Expected token } instead of token ") + *j);
			return std::vector<std::shared_ptr<gt::FeedGroup>>();
		}
		i = std::find_first_of(j + 1, sData.end(), tokens.begin(), tokens.end());;

		// parse filters
		string filter;
		j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		std::string filtername = std::string(i, j);
		if(*j != '|')
		{
			gt::Log::Debug(std::string("Expected token | instead of token ") + *j + " in filter expression");
			return std::vector<std::shared_ptr<gt::FeedGroup>>();
		}
		i = j + 1;
		j = std::find_first_of(i, sData.end(), tokens.begin(), tokens.end());
		std::string regex = std::string(i, j);
		feedg->filters[filtername] = regex;

		// TODO: finish from here
		while(*j == ',')
		{
		}

		if(*j != '}')
		{
			gt::Log::Debug(std::string("Expected token } instead of token ") + *j + " in filter expression");
			return std::vector<std::shared_ptr<gt::FeedGroup>>();
		}

	}

	return ret;
}
bool gt::FeedGroup::contains(libtorrent::feed_handle fh)
{
	for(auto f : m_feeds)
		if(*f == fh)
			return true;
	return false;
}

gt::FeedGroup::operator string()
{
	// TODO: check if it works
	std::string str;
	// group name
	str += "[" + name + "]\n";
	auto i = m_feeds.begin();

	// feed urls
	str += "{\n\t";
	while(1)
	{
		str += (*i)->settings().url;
		if(++i == m_feeds.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n";

	//filters
	auto pair = filters.begin();
	str += "{\n\t";
	while(1)
	{
		str += pair->first + "|" + pair->second;
		if(++pair == filters.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n";

	//functions
	auto fun = functions.begin();
	str += "{\n\t";
	while(1)
	{
		str += *fun;
		if(++fun == functions.end()) break;
		str += ",\n\t";
	}
	str += "\n}\n\n";

	return str;
}
