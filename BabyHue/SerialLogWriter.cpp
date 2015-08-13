//
//  SerialLogWriter.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include <Bridge.h>
#include "SerialLogWriter.h"

SerialLogWriter::SerialLogWriter(int baudrate, bool waitForPort) {
    Serial.begin(baudrate);
    
    // Check if we have to wait for a port
    if (waitForPort) {
        while (!Serial) {
            delay(100);
        }
    }
}

size_t SerialLogWriter::write(const char str[]) {
    if (Serial) {
        return Serial.print(str);
    } else {
        return 0;
    }
}

size_t SerialLogWriter::write(char c) {
    if (Serial) {
        return Serial.print(c);
    } else {
        return 0;
    }
}

size_t SerialLogWriter::writeln(const char c[]) {
    if (Serial) {
        return Serial.println(c);
    } else {
        return 0;
    }
}

size_t SerialLogWriter::writeln(const String &s) {
    if (Serial) {
        return Serial.println(s);
    } else {
        return 0;
    }
}

void SerialLogWriter::flush() {
    if (Serial) {
        Serial.flush();
    }
}

void SerialLogWriter::wait() {
    while (Serial._rx_buffer_head != Serial._rx_buffer_tail) delay(10);
}