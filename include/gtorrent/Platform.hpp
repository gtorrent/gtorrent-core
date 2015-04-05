#pragma once

#include <string>
#include <memory>

namespace gt
{
	class Torrent;

	/**
	* Defines platform specific functions that will be implemented for different environments
	*/
	namespace Platform
	{
		/**
		 * Gets the home directory of the user.
		 */
		std::string getHomeDir();

		/**
		* Safely retrieve environment variables
		* @param env The name of the XDG variable to get
		* @param default_value value returned if XDG variable hasn't been defined
		*/
		std::string getEnv(std::string env, std::string default_value);

		/**
		 * Returns $XDG_DOWNLOAD_DIR for platform
		 * Default: ~/Downloads
		 */
		std::string getXdgSavePath();

		/**
		 * Returns $XDG_CONFIG_HOME for platform
		 * Default: ~/.config
		 */
		std::string getXdgConfigPath();

		/**
		 * Gets default path of where the config and session files should be saved.
		 * Default Linux: $XDG_CONFIG_HOME/GTORRENT_CONFIG_HOME
		 * Default Windows: %Appdata%/gtorrent
		 */
		std::string getDefaultConfigPath();

		/**
		* Get location of the binary the current process has been loaded from
		* On linux, returns the path of the link /proc/self/exe
		*/
		std::string getExecutablePath();

		/**
		 * Gets the native file separator
		 */
		char getFileSeparator();

		/**
		 * Creates a directory
		 * @returns the value of the equivalent POSIX syscall
		 */
		int makeDir(std::string, unsigned);

		/**
		 * Checks if a directory exists.
		 */
		bool dirExist(std::string);

		void associate(bool, bool);

		void makeSharedFile();

		bool processIsUnique();

		void writeSharedData(std::string info);

		std::string readSharedData();

		void disableSharedData();

		void openTorrent(std::shared_ptr<gt::Torrent> t, int index = -1, bool folder = false);
	}
}
