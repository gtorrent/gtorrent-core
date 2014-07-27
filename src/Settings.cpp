#include "Settings.hpp"
#include "Platform.hpp"
#include <fstream>
#include <sstream>

std::map<std::string, std::string> gt::Settings::settings;

bool gt::Settings::parse(const std::string &path)
{
	std::ifstream configFile(gt::Platform::getDefaultConfigPath() + path);
	if (!configFile)
		return true;
	while (!configFile.eof())
	{
		std::string key;
		std::string value;

		// split a line
		getline(configFile, key);
		unsigned sep = key.find('=');
		if (sep != std::string::npos)
		{
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

template<typename T>
T gt::Settings::getOption(const std::string &key)
{
	std::stringstream opt(settings[key]);
	T value;
	opt >> value;
	return value;
}

template<typename T>
void gt::Settings::setOption(const std::string &key, T value)
{
	std::stringstream opt;
	opt << value;
	settings[key] = opt.str();
}
