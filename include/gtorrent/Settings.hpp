#pragma once

#include <map>
#include <string>

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
		template<typename T>
		static T getOption(const std::string &key);
		template<typename T>
		static void setOption(const std::string &key, T value);
	private:
		static std::map<std::string, std::string> settings;
	};
}
