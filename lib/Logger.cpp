#include "Logger.h"

namespace Hydra
{
    Log::Log(string newName, string newFilename)
    {
        name = newName;
        filename = newFilename + ".txt";
        log("Init creation of logfile " + filename);
    }
    void Log::log(string message, logFlag flag)
    {
        //Output time for logging purposes
        time_t rawTime;
        tm* timeInfo;
        time(&rawTime);
        timeInfo = localtime(&rawTime);

        string flagText;
        switch (flag)
        {
        case error:
            flagText = "[ERROR]\t\t";
            break;
        case info:
            flagText = "[INFO]\t\t";
            break;
        case resource:
            flagText = "[RESOURCE]\t";
            break;
        case hydsys:
            flagText = "[SYSTEM]\t";
            break;
        default:
            flagText = "[NOFLAG]\t";
            break;
        }

        stringstream logEntry;
        logEntry << "[" << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec << "]\t";
        logEntry << flagText;
        logEntry << message;
        logBuffer.push_back(logEntry.str());
        if (logBuffer.size() >= MAX_LOGBUFFER_ENTRIES)
            flushBuffer();
    }
    void Log::flushBuffer()
    {
        //TODO: Have this operate in a separate thread (for efficiency)
        ofstream file;
        file.open(filename);
        for (auto iter = logBuffer.begin(); iter != logBuffer.end(); iter++)
            file << *iter << endl;
    }

    Logger* Logger::instance = nullptr;
    Logger* Logger::getInstance()
    {
        if (instance == nullptr)
            instance = new Logger;
        return instance;
    }
    Logger::Logger()
    {
        newLog("sysLog", "/sysLog");
    }
    Logger::~Logger() {}
    void Logger::log(string message, logFlag flag, string name)
    {
        //Find the correct log, then log the message with it
        for (auto iter = logFiles.begin(); iter != logFiles.end(); iter++)
        {
            if (iter->name == name)
            {
                iter->log(message, flag);
                return;
            }
        }
        //At this point, it is confirmed that no log file exists.
        log("Cannot find log " + name + ", creating new one at." + name + ".txt", info);
        Log newLog(name, name);
        newLog.log(message, flag);
        logFiles.push_back(newLog);
    }
    void Logger::newLog(string name, string filename)
    {
        //Check for duplicate logs
        for (auto iter = logFiles.begin(); iter != logFiles.end(); iter++)
        {
            if (iter->name == name)
                return; //Duplicate log found.
        }
        Log _newLog(name, filename);
        logFiles.push_back(_newLog);
    }
    Log* Logger::getLog(string name)
    {
        for (auto iter = logFiles.begin(); iter != logFiles.end(); iter++)
        {
            if (iter->name == name)
                return &(*iter);
        }
        return nullptr;
    }
    void Logger::flushLogBuffers()
    {
        for (auto iter = logFiles.begin(); iter != logFiles.end(); iter++)
            iter->flushBuffer();
    }
};
