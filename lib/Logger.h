#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <sstream>
using std::endl;
using std::string;
using std::vector;
using std::ofstream;
using std::stringstream;

//Logger files were ripped out of a game dev library. Some things here (such as flags) are customized more for game dev than for robotics.
//Note that as the roboRIO is Linux-based (or something really close) a "/" needs to be at the beginning of the filename in order to log anything.
namespace Hydra
{
	#define MAX_LOGBUFFER_ENTRIES 10 //How many entries the log buffer stores before outputting all entries to a file. Change this to 0 for unstable (i.e. can crash) environments.

	enum logFlag {error, hydsys, info, resource}; //All possible flags that could be used. Default is hydsys. 
	class Log
	{
	public:
		Log(string newName, string newFilename); //!< Creates a new log with this name at this filename. Do NOT append .txt to the filename, it does it automatically
		void log(string message, logFlag flag = hydsys); //!< Writes a message (with timestamp) to the log with the specified flag
		void flushBuffer(); //!< Forces the contents of the buffer to output to file.
	private:
		vector<string> logBuffer;
		string filename;
		string name;
		friend class Logger;
	};

	//Logger class automatically creates its own sysLog log upon creation.
	class Logger
	{
	public:
		friend class Log;
		void log(string message = "Default log output", logFlag flag = hydsys, string name = "sysLog"); //!< Logs something in the logger of the given name
		void newLog(string name = "sysLog", string filename = "/sysLog"); //!< Creates a new log with the specified name and filename
		Log* getLog(string name); //!< Returns a pointer to the log with the specified name. If no such log exists, returns nullptr.
		void flushLogBuffers(); //!< Manually flushes all log buffers.
		static Logger* getInstance();
	private:
		static Logger* instance;
		vector<Log> logFiles;
		Logger();
		~Logger();
	};
};
