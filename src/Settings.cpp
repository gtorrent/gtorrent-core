#include "Settings.hpp"
#include "Platform.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <libtorrent/version.hpp>
#define xstrmacro(s) strmacro(s)
#define strmacro(s) #s
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


std::string gt::Settings::getOptionAsString(const std::string &key)
{
	return settings[key]; // >yfw exceptions are harmful
}

int gt::Settings::getOptionAsInt(const std::string &key)
{
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

void gt::Settings::setDefaults()
{
	// these will be overwritten if the keys are found in the config file

	settings["SavePath"] = gt::Platform::getDefaultSavePath();
	settings["FileAssociation"] = "-1";

	settings["PausedForeGroundColor"]      = "#F08080";
	settings["PausedBackGroundColor"]      = "#800000";
	settings["QueuedForeGroundColor"]      = "#00BFFF";
	settings["QueuedBackGroundColor"]      = "#FFFFFF";
	settings["SeedingForeGroundColor"]     = "#1E90FF";
	settings["SeedingBackGroundColor"]     = "#ADD8E6";
	settings["MetadataForeGroundColor"]    = "#228B22";
	settings["MetadataBackGroundColor"]    = "#7FFFD4";
	settings["FinishedForeGroundColor"]    = "#ADD8E6";
	settings["FinishedBackGroundColor"]    = "#483D8B";
	settings["ResumingForeGroundColor"]    = "#6495ED";
	settings["ResumingBackGroundColor"]    = "#FAF0E6";
	settings["CheckingForeGroundColor"]    = "#DAA520";
	settings["CheckingBackGroundColor"]    = "#FFFACD";
	settings["AllocatingForeGroundColor"]  = "#FF7F50";
	settings["AllocatingBackGroundColor"]  = "#FAFAD2";
	settings["DownloadingForeGroundColor"] = "#228B43";
	settings["DownloadingBackGroundColor"] = "#FFFFFF";

	//////////////////// Below this line, options haven't been tested yet. ////////////////////////////
	settings["GraphUploadCurveColor"] = "red";
	settings["GraphDownloadCurveColor"] = "green";
	settings["GraphGridColor"] = "grey";
	settings["ShowLegend"] = "Yes";

	settings["ProxyHost"] = "";
	settings["ProxyType"] = "None"; // Can be: HTTP, SOCKS4, SOCKS5, I2P
	settings["ProxyPort"] = "8080";
	settings["ProxyCredentials"] = "user:pass";

	settings["CacheSize"] = "0"; // Multiple of 16KB blocks // defaults a 1/8 of total RAM !!!
	settings["CachedChunks"] = ""; // Number of blocks allocated at a time
	settings["CacheExpiry"] = ""; // Number of second elapsed before flushing to disk
	settings["AnonymousMode"] = "No";
	settings["OverrideSettings"] = "No"; // Can be set to Minimal, HighPerformanceSeeding, and No.

	//Where an upload limit is required, it should be as close as possible to the real upload limit
	/*
	 * Default: Unchoke slots = Max upload slots
	 * AutoExpand: Unchoke slots = Max upload slots, where unchock slots grow as long as upload bandwidth is not saturated, , requires to set up a global upload limit
	 * RateBased: Unchoke slot determined by the ration of a peer
	 * BitTyrant: :^) trigger warning: Looks for the best peers to maximize download speed, requires to set up a global upload limit
	 */
	settings["ChokingAlgorithm"] = "Default";
	/* Only applies to BitTyrant */
	// not sure about how it werks concretly
	settings["DefaultReciprocationRate"] = "14"; // Unit is download speed in KB/s
	settings["IncreaseReciprocationRate"] = "20"; // In percents
	settings["DecreaseReciprocationRate"] = "3";

	/*
	 * RoundRobins: Distribute the upload fairly.
	 * FastestUpload: Unchoke the fastest peers
	 * AntiLeech: Prioritize peers who just started or about to finish, making leeches in the middle share between them
	 */
	settings["SeedChokingAlgorithm"] = "RoundRobin";


	//used for the tracker HTTP requests, and also sent to client supporting the useragent extension
	settings["UserAgent"] = "gTorrent/" xstrmacro(GTORRENT_VERSION_MAJOR) "." xstrmacro(GTORRENT_VERSION_MINOR) "." xstrmacro(GTORRENT_VERSION_PATCH) " libtorrent/" xstrmacro(LIBTORRENT_VERSION_MAJOR)"." xstrmacro(LIBTORRENT_VERSION_MINOR) "." xstrmacro(LIBTORRENT_VERSION_TINY);

	// suggest piece that are in the disk cache, other supported value is No
	settings["PieceSuggestion"] = "Yes";

	/* 0 means unlimited, all units are in bytes per second unless specified otherwise */
	settings["GlobalUploadLimit"] = "0";
	settings["GlobalDownloadLimit"] = "0";

	settings["DHTUploadLimit"] = "4000"; // it is the default in libtorrent, should be set higher for seedboxes
	// Set to yes if you want to count in the amount of redundant bytes downloaded that is sent to the tracker
	settings["ReportTrueDownloaded"] = "No";
	settings["DefaultSequentialDownloading"] = "No"; // When is Yes, will set seq by default only if the torrent has a single file that ends in the below list 
	settings["SequentialDownloadExtensions"] = "mkv/mp3/flac/mp4/mp5/avi";

    //////////////////// Below this line, the options aren't implemented into core yet. ////////////////////////

}
