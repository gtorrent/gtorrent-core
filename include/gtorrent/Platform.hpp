#pragma once
#include <string>

using namespace std;

namespace gt
{
	namespace Platform
	{
		// TODO Better names needed. Suggestions welcome.
		string getDefaultSavePath();
		string getDefaultConfigPath();
		string getHomeDir();
		bool checkDirExist(string);
	}
}
