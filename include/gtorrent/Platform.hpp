#pragma once

using namespace std;

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include "libtorrent.hpp"
#include "Torrent.hpp"

namespace gt
{
	namespace Platform
	{
		static string getDefaultSavePath();
		static bool checkDirExist(string);
	}
}
