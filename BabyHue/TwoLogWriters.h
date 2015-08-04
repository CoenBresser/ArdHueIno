//
//  TwoLogWriters.h
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__TwoLogWriters__
#define __BabyHue__TwoLogWriters__

#include "Logger.h"

class TwoLogWriters : public iLogWriter {
public:
    TwoLogWriters(iLogWriter& first, iLogWriter& second) : first(&first), second(&second) { };
    
    size_t write(const char str[]);
    size_t write(char c);
    size_t writeln(const char c[]);
    size_t writeln(const String &s);
    void flush();

private:
    iLogWriter *first;
    iLogWriter *second;
};

#endif /* defined(__BabyHue__TwoLogWriters__) */
