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

namespace Hydra
{
    #define MAX_LOGBUFFER_ENTRIES 10

    enum logFlag {error, hydsys, info, resource};
    class Log
	{
    public:
	    Log(string newName, string newFilename);
	    void log(string message, logFlag flag = hydsys); //!< Writes a message (with timestamp) to the log with the specified flag
	    void flushBuffer();
	private:
	    vector<string> logBuffer;
        string filename;
		string name;
		friend class Logger;
	};
    class Logger
	{
	public:
		friend class Log;
	    void log(string message = "Default log output", logFlag flag = hydsys, string name = "sysLog"); //!< Logs something in the logger of the given name
	    void newLog(string name = "sysLog", string filename = "/sysLog"); //!< Creates a new log with the specified name and filename
	    Log* getLog(string name); //!< Returns a pointer to the log with the specified name.
	    void flushLogBuffers(); //!< Manually flushes all log buffers.
		static Logger* getInstance();
	private:
		static Logger* instance;
		vector<Log> logFiles;
		Logger();
		~Logger();
	};
};
