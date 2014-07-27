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
	static string getDefaultSavePath();
	static bool checkDirExist(string);
	static bool isMagnetLink(string const& link);
}
