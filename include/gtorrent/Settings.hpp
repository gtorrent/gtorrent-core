#pragma once

#include <map>
#include <string>

#define SAVEPATH_OPTION_KEY "SavePath"

namespace gt
{
	/**
	* TODO Settings shouldn't be treated like a static class
	* You should be able to support multiple instances of Settings.
	* If all these functions are static, what's the point of having Settings be a fucking class
	* instead of a namespace?
	* It's also practical so that you can separate file/category specific settings without mixing up all the values
	* E.g: "~/.config/gtorrent/config" for gtorrent-core settings
	*      "~/.config/gtorrent/config_gtk" for gtorrent-gtk settings
	*/
	class Settings
	{
	public:

		static bool parse(const std::string &path);

		static bool save(const std::string &path);

		static bool optionExists(const std::string &key);

		static int getOptionAsInt(const std::string &key);

		static std::string getOptionAsString(const std::string &key);

		static void setOption(const std::string &key, int value);

		static void setOption(const std::string &key, std::string value);

		static void setDefaults();

		static std::map<std::string, std::string> settings;
	};
}
