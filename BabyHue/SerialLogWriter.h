//
//  SerialLogWriter.h
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__SerialLogWriter__
#define __BabyHue__SerialLogWriter__

#include "Logger.h"

class SerialLogWriter : public iLogWriter {
public:
    SerialLogWriter(int baudrate = 9600, bool waitForPort = false);

    size_t write(const char str[]);
    size_t write(char c);
    size_t writeln(const char c[]);
    size_t writeln(const String &s);
    void flush();
};

#endif /* defined(__BabyHue__SerialLogWriter__) */
