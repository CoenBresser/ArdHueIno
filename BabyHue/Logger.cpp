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
    if (logLevel <= level && Serial) {
        Serial.print("DMP - ");
        while (s.available()) {
            char c = s.read();
            Serial.print(c);
        }
        Serial.println("");
        Serial.flush();
    } else {
        s.flush();
    }
}

void Logger::trace(String trace) {
    if (logLevel <= Trace && Serial) {
        Serial.println("TRC - " + trace);
        Serial.flush();
    }
}

void Logger::debug(String debug) {
    if (logLevel <= Debug && Serial) {
        Serial.println("DBG - " + debug);
        Serial.flush();
    }
}

void Logger::info(String info) {
    if (logLevel <= Info && Serial) {
        Serial.println("INF - " + info);
        Serial.flush();
    }
}

void Logger::error(String error) {
    if (logLevel <= Error && Serial) {
        Serial.println("ERR - " + error);
        Serial.flush();
    }
}

void Logger::fail(String fail) {
    if (logLevel <= Fail && Serial) {
        Serial.println("FAIL! - " + fail);
        Serial.flush();
    }
}