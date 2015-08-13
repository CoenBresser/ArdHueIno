//
//  Logger.h
//  BabyHue
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__Logger__
#define __BabyHue__Logger__

#include <Bridge.h>

/**
 * Interface to implement when writing a logwriter
 */
class iLogWriter {
public:
    virtual size_t write(const char str[]) = 0;
    virtual size_t write(char c) = 0;
    virtual size_t writeln(const char c[]) = 0;
    virtual size_t writeln(const String &s) = 0;
    virtual void flush() = 0;
    virtual void wait() = 0;
};

#define LOG_LEVEL_DUMP 7
#define LOG_LEVEL_TRACE 6
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_FAIL 1
#define LOG_LEVEL_NONE 0

/**
 * Logger class to write to a log destination (Serial, FileIO)
 *
 * None of the methods are blocking so if a serial port is not available, it will be skipped
 * Note that initialization of the serial port is not done here
 */
class Logger_ {
public:
    
    Logger_(int logLevel = LOG_LEVEL_INFO) : started(true), logLevel(logLevel) { }
    
    void dumpStream(Stream& stream, int logLevel);
    
    void trace(const String& traceMsg);
    void debug(const String& debugMsg);
    void info(const String& infoMsg);
    void warn(const String& warnMsg);
    void error(const String& errorMsg);
    void fail(const String& failMsg);
    
    void trace(const char *traceMsg, const char *data = NULL);
    void debug(const char *debugMsg, const char *data = NULL);
    void info(const char *infoMsg, const char *data = NULL);
    void warn(const char *warnMsg, const char *data = NULL);
    void error(const char *errorMsg, const char *data = NULL);
    void fail(const char *failMsg, const char *data = NULL);
    
    void setLogLevel(int logLevel);
    int getLogLevel();
    
    // Method to register a log writers. There is room for 2 at the moment
    // TODO: convert to stream
    void registerLogWriter(iLogWriter& writer);
    
private:
    boolean started;
    int logLevel;
    iLogWriter *logWriter;
    
    void write(int checkLvl, const char *lvlMsg, const char *msg, const char *data);
};

extern Logger_ Logger;


#endif /* defined(__BabyHue__Logger__) */
