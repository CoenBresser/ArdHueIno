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
    logWriter->wait();
    if (logLevel >= level) {
        logWriter->write("DMP - ");
        // TODO: streamline this so it reads chunks i.o. characters
        while (s.available()) {
            char c = s.read();
            logWriter->write(c);
        }
        logWriter->writeln(" - END");
        logWriter->flush();
    } else {
        s.flush();
    }
}

void Logger_::trace(const String& traceMsg) {
    trace(traceMsg.c_str());
}

void Logger_::debug(const String& debugMsg) {
    debug(debugMsg.c_str());
}

void Logger_::info(const String& infoMsg) {
    info(infoMsg.c_str());
}

void Logger_::warn(const String& warnMsg) {
    warn(warnMsg.c_str());
}

void Logger_::error(const String& errorMsg) {
    error(errorMsg.c_str());
}

void Logger_::fail(const String& failMsg) {
    fail(failMsg.c_str());
}

void Logger_::write(int checkLvl, const char *lvlMsg, const char *msg, const char *data) {
    if (!logWriter) {
        return;
    }
    logWriter->wait();
    if (logLevel >= checkLvl) {
        logWriter->write(lvlMsg);
        logWriter->write(msg);
        if (data) logWriter->write(data);
        logWriter->writeln("");
        logWriter->flush();
    }
}

void Logger_::trace(const char *traceMsg, const char *data) {
    write(LOG_LEVEL_TRACE, "TRC - ", traceMsg, data);
}

void Logger_::debug(const char *debugMsg, const char *data) {
    write(LOG_LEVEL_DEBUG, "DBG - ", debugMsg, data);
}

void Logger_::info(const char *infoMsg, const char *data) {
    write(LOG_LEVEL_INFO, "INF - ", infoMsg, data);
}

void Logger_::warn(const char *warnMsg, const char *data) {
    write(LOG_LEVEL_WARN, "WRN - ", warnMsg, data);
}

void Logger_::error(const char *errorMsg, const char *data) {
    write(LOG_LEVEL_ERROR, "ERR - ", errorMsg, data);
}

void Logger_::fail(const char *failMsg, const char *data) {
    write(LOG_LEVEL_FAIL, "FAIL! - ", failMsg, data);
}

void Logger_::registerLogWriter(iLogWriter& writer) {
    logWriter = &writer;
}

Logger_ Logger;
