#pragma once

using namespace std;

#include <string>
#include <fstream>

namespace gt
{
	class Log
	{
	public:
		static string getTimeStamp();
		
		
		/** stream for sending debug messages to the log **/
		/* Usage:
			
			gt::Log::Debug() << "debug messages";
			
			
		   NOTE: do not use 'endl'!
		*/
		class Debug
		{
		public:
			Debug();
			~Debug();
			
			template <typename T>
			Debug& operator << (const T& arg) {
				m_logFile << arg;
				cout << arg;
				return *this;
			}
			
		private:
			fstream m_logFile;
		};
	};
}
