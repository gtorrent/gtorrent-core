#pragma once

#include <map>
#include <string>

#define SAVEPATH_OPTION_KEY "SavePath"

namespace gt
{
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
