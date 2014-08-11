#pragma once

#include <string>
#include <memory>

using namespace std;

namespace gt
{
	class Torrent;

	namespace Platform
	{
		// TODO Better names needed. Suggestions welcome.

		/**
		 * Gets default path of where Downloads should be saved.
		 * TODO Attempts to use XDG_DESKTOP_DIR if it is defined
		 * Usually: ~/Downloads
		 */
		string getDefaultSavePath();
		/**
		 * Gets default path of where the config and session files should be saved.
		 * TODO Attempts to use XDG_CONFIG_HOME/gtorrent if it is defined
		 * Usually: ~/.config/gtorrent
		 */
		string getDefaultConfigPath();
		/**
		 * Gets the home directory of the user.
		 */
		string getHomeDir();
		/**
		 * Gets the OS's file separator
		 */
		char   getFileSeparator();
		/**
		 * Creates a directory
		 * returns the values of the POSIX syscall.
		 */
		int    makeDir(std::string, mode_t);
		/**
		 * Checks if a directory exists.
		 */
		bool   checkDirExist(string);

		void   associate(bool, bool);

		void   makeSharedFile();
		bool   processIsUnique();
		void   writeSharedData(string info);
		string readSharedData();
		void   disableSharedData();
		void   openTorrent(std::shared_ptr<gt::Torrent> t);
		string getExecutablePath();

	}
}
