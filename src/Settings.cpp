#include "Settings.hpp"
#include "Platform.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::map<std::string, std::string> gt::Settings::settings;

bool gt::Settings::parse(const std::string &path)
{
	gt::Settings::setDefaults();
	std::ifstream configFile(gt::Platform::getDefaultConfigPath() + path);
	if (!configFile)
		return true;
	while (!configFile.eof())
	{
		std::string key;
		std::string value;

		// split a line
		getline(configFile, key);
		if (key.find('=') != std::string::npos)
		{
			unsigned sep = key.find('=');
			value = key.substr(sep + 1);
			key = key.substr(0, sep);

			// remove whitespace
			while (isblank(key[key.length() - 1]))
				key.pop_back();
			while (isblank(value[0]))
				value = value.substr(1);

			settings[key] = value;
		}
	}
	configFile.close();
	return false;
}

bool gt::Settings::save(const std::string &path)
{
	std::ofstream configFile(gt::Platform::getDefaultConfigPath() + path, ios_base::trunc);// old configfile is destroyed
	if(!configFile)
		return true;
	for(auto i = settings.begin(); i != settings.end(); ++i)
		configFile << i->first << " = " << i->second << std::endl;
	configFile.close();
	return false;
}

// TODO Please, for the love of god, let's not use exceptions.
// I have no bloody clue what to do in C++, but if I were to do it
// in C, I would have something like this:
//  int get_opt_int(const char* key, int* val);
// where get_opt_int returns 0 on error, and val is the pointer to
// what you want changed
// -- inuoppai
std::string gt::Settings::getOptionAsString(const std::string &key)
{
	return settings[key]; // >yfw exceptions are harmful
}

int gt::Settings::getOptionAsInt(const std::string &key)
{
	// FIXME I wrote a function that checks if an option exists. -- inuoppai
	auto i = settings.find(key);
	if(i == settings.end())
		throw std::runtime_error("No such option.");
	std::stringstream opt(i->second);
	int value;
	opt >> value;
	return value;
}

void gt::Settings::setOption(const std::string &key, int value)
{
	std::stringstream opt;
	opt << value;
	settings[key] = opt.str();
}

void gt::Settings::setOption(const std::string &key, std::string value)
{
	settings[key] = value;
}

bool gt::Settings::checkOptionExist(const std::string key)
{
	auto i = settings.find(key);
	if(i == settings.end())
		return false;
	return true;
}

void gt::Settings::setDefaultOption(const std::string key, const std::string val)
{
	if checkOptionExist(key)
		return;

	settings[key] = val;
}

// TODO Fix name changes throughout project. i.e key -> core.key
void gt::Settings::setDefaults()
{
	// these will be overwritten if the keys are found in the config file
	// TODO Well then, write a function that checks to see if the key is defined
	// first before attempting to overwrite, baka inu! -- inuoppai

	settings["core.SavePath"                      ] = gt::Platform::getDefaultSavePath();
	settings["core.FileAssociation"               ] = "-1";

	// Below this line, options haven't been tested yet.
	// TODO Move this out of core
	settings["gtk.GraphUploadCurveColor"          ] = "red";
	settings["gtk.GraphDownloadCurveColor"        ] = "green";
	settings["gtk.GraphGridColor"                 ] = "grey";
	settings["gtk.ShowLegend"                     ] = "Yes";

	settings["core.ProxyHost"                     ] = "";
	settings["core.ProxyType"                     ] = "None"; // Can be: HTTP, SOCKS4, SOCKS5, I2P
	settings["core.ProxyPort"                     ] = "8080";
	settings["core.ProxyCredentials"              ] = "user:pass";

	settings["core.CacheSize"                     ] = "0"; // Multiple of 16KB blocks // defaults a 1/8 of total RAM !!!
	settings["core.CachedChunks"                  ] = ""; // Number of blocks allocated at a time
	settings["core.CacheExpiry"                   ] = ""; // Number of second elapsed before flushing to disk
	settings["core.AnonymousMode"                 ] = "No";

	// Below this line, the options aren't implemented into core yet. //
	settings["core.DefaultSequentialDownloading"  ] = "No"; // When is Yes, will set seq by default only if the torrent has a single file that ends in the below list
	settings["core.SequentialDownloadExtensions"  ] = "mkv|mp3|flac|mp4|mp5|avi";

	settings["core.OverrideSettings"              ] = "No"; // Can be set to Minimal, HighPerformanceSeeding, and No.

	/*
	 * Default: Unchoke slots = Max upload slots
	 * AutoExpand: Unchoke slots = Max upload slots, where unchock slots grow as long as upload bandwidth is not saturated
	 * RateBased: Unchoke slot determined by the ration of a peer
	 * BitTyrant: :^) trigger warning: Looks for the best peers to maximize download speed, requires to set up a global upload limit
	 */
	settings["core.ChokingAlgorithm"              ] = "Default";

	/* Only applies to BitTyrant */
	// not sure about how it werks concretly
	settings["core.DefaultReciprocationRate"      ] = "14"; // Unit is download speed in KB/s
	settings["core.IncreaseReciprocationRate"     ] = "20"; // In percents
	settings["core.DecreaseReciprocationRate"     ] = "3";


	/*
	 * RoundRobins: Distribute the upload fairly.
	 * FastestUpload: Unchoke the fastest peers
	 * AntiLeech: Prioritize peers who just started or about to finish, making leeches in the middle share between them
	 */
	settings["core.SeedChokingAlgorithm" ] = "RoundRobin";

	settings["core.UserAgent"            ] = "gTorrent/0.0.2 libtorrent/0.16.17"; //used for the tracker HTTP requests, and also sent to client supporting the useragent extension

	settings["core.PieceSuggestion"      ] = "Yes"; // suggest piece that are in the disk cache, other supported value is No

	/* 0 means unlimited, all units are in bytes per second unless specified otherwise */
	settings["core.GlobalUploadLimit"   ] = "0";
	settings["core.GlobalDownloadLimit" ] = "0";
	settings["core.DHTUploadLimit"      ] = "4000"; // it is the default in libtorrent, should be set higher for seedboxes

	// Set to yes if you want to count in the amount of redundant bytes downloaded.
	settings["core.ReportTrueDownloaded"] = "No";
}
