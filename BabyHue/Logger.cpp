//
//  Logger.cpp
//  BabyHue
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "Logger.h"

void Logger::setLogLevel(LogLevel level) {
    logLevel = level;
}

Logger::LogLevel Logger::getLogLevel() {
    return logLevel;
}

void Logger::dumpStream(Stream &s, LogLevel level) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= level && Serial) {
        logWriter->write("DMP - ");
        while (s.available()) {
            char c = s.read();
            logWriter->write(c);
        }
        logWriter->writeln("");
        logWriter->flush();
    } else {
        s.flush();
    }
}

void Logger::trace(String trace) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= Trace && Serial) {
        logWriter->writeln("TRC - " + trace);
        logWriter->flush();
    }
}

void Logger::debug(String debug) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= Debug && Serial) {
        logWriter->writeln("DBG - " + debug);
        logWriter->flush();
    }
}

void Logger::info(String info) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= Info && Serial) {
        logWriter->writeln("INF - " + info);
        logWriter->flush();
    }
}

void Logger::error(String error) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= Error && Serial) {
        logWriter->writeln("ERR - " + error);
        logWriter->flush();
    }
}

void Logger::fail(String fail) {
    if (!logWriter) {
        return;
    }
    if (logLevel <= Fail && Serial) {
        logWriter->writeln("FAIL! - " + fail);
        logWriter->flush();
    }
}

void Logger::registerLogWriter(iLogWriter& writer) {
    logWriter = &writer;
}