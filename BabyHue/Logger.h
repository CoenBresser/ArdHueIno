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
    
private:
    boolean started;
    LogLevel logLevel;
};

#endif /* defined(__BabyHue__Logger__) */
