#pragma once

#include <string>
#include <memory>

namespace gt
{
	class Torrent;

	namespace Platform
	{
		// TODO Better names needed. Suggestions welcome.

		/**
		 * Gets default path of where Downloads should be saved.
		 * TODO Attempt to use XDG_DESKTOP_DIR if it is defined
		 * Usually: ~/Downloads
		 */
		std::string getDefaultSavePath();

		/**
		 * Gets default path of where the config and session files should be saved.
		 * TODO Attempt to use XDG_CONFIG_HOME/gtorrent if it is defined
		 * Usually: ~/.config/gtorrent
		 */
		std::string getDefaultConfigPath();

		/**
		 * Gets the home directory of the user.
		 */
		std::string getHomeDir();

		/**
		 * Gets the OS's file separator
		 */
		char getFileSeparator();

		/**
		 * Creates a directory
		 * returns the values of the POSIX syscall.
		 */
		int makeDir(std::string, unsigned);
		/**
		 * Checks if a directory exists.
		 */
		bool checkDirExist(std::string);

		void associate(bool, bool);

		void makeSharedFile();

		bool processIsUnique();

		void writeSharedData(std::string info);

		std::string readSharedData();

		void disableSharedData();

		void openTorrent(std::shared_ptr<gt::Torrent> t, int index = -1, bool folder = false);

		std::string getExecutablePath();
	}
}
