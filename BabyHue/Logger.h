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
};

/**
 * Logger class to write to a log destination (Serial, FileIO)
 *
 * None of the methods are blocking so if a serial port is not available, it will be skipped
 * Note that initialization of the serial port is not done here
 */
class Logger {
public:
    enum LogLevel { Trace, Debug, Info, Error, Fail, None };
    
    Logger(LogLevel level = Info) : started(true) {
        logLevel = level;
    }
    
    void dumpStream(Stream& stream, LogLevel level);
    
    void trace(String trace);
    void debug(String debug);
    void info(String info);
    void error(String error);
    void fail(String fail);
    
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel();
    
    // Method to register a log writers. There is room for 2 at the moment
    void registerLogWriter(iLogWriter& writer);
    
private:
    boolean started;
    LogLevel logLevel;
    iLogWriter *logWriter;
};

#endif /* defined(__BabyHue__Logger__) */
