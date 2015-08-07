//
//  Logger.cpp
//  BabyHue
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "Logger.h"

void Logger_::setLogLevel(int level) {
    logLevel = level;
}

int Logger_::getLogLevel() {
    return logLevel;
}

void Logger_::dumpStream(Stream &s, int level) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= level && Serial) {
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

void Logger_::trace(String trace) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= LOG_LEVEL_TRACE && Serial) {
        logWriter->writeln("TRC - " + trace);
        logWriter->flush();
    }
}

void Logger_::debug(String debug) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= LOG_LEVEL_DEBUG && Serial) {
        logWriter->writeln("DBG - " + debug);
        logWriter->flush();
    }
}

void Logger_::info(String info) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= LOG_LEVEL_INFO && Serial) {
        logWriter->writeln("INF - " + info);
        logWriter->flush();
    }
}

void Logger_::error(String error) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= LOG_LEVEL_ERROR && Serial) {
        logWriter->writeln("ERR - " + error);
        logWriter->flush();
    }
}

void Logger_::fail(String fail) {
    if (!logWriter) {
        return;
    }
    if (logLevel >= LOG_LEVEL_FAIL && Serial) {
        logWriter->writeln("FAIL! - " + fail);
        logWriter->flush();
    }
}

void Logger_::registerLogWriter(iLogWriter& writer) {
    logWriter = &writer;
}

Logger_ Logger;
