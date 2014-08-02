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
		static inline bool optionExists(const std::string &key)
		{
			return settings.count(key) > 0;
		}
		static int getOptionAsInt(const std::string &key);
		static std::string getOptionAsString(const std::string &key);
		static void setOption(const std::string &key, int value);
		static void setOption(const std::string &key, std::string value);
		static bool checkOptionExist(const std::string key);
		static void setDefaultOption(const std::string key, const std::string val);
		static void setDefaults();
		static std::map<std::string, std::string> settings;
	};
}
