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

	settings["GraphUploadCurveColor"] = "red";
	settings["GraphDownloadCurveColor"] = "green";
	settings["GraphGridColor"] = "grey";
	settings["ShowLegend"] = "Yes";

	// Below this line, the options aren't implemented yet. //
	settings["DefaultSequentialDownloading"] = "No"; // When is Yes, will set seq by default only if the torrent has a single file that ends in the below list 
	settings["SequentialDownloadExtensions"] = "mkv|mp3|flac|mp4|mp5|avi";
	
}
