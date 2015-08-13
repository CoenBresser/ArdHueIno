//
//  FileLogWriter.h
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__FileLogWriter__
#define __BabyHue__FileLogWriter__

#include <FileIO.h>
#include "Logger.h"

class FileLogWriter : public iLogWriter {
public:
    // The default is in the Yun RAM!!
    FileLogWriter(const char *fileName = "/tmp/arduino-log.txt") : logFileName(fileName) {
        FileSystem.begin();
    };
    
    size_t write(const char str[]);
    size_t write(char c);
    size_t writeln(const char c[]);
    size_t writeln(const String &s);
    void flush();
    void wait();

private:
    const char *logFileName;
    File getFile();
};

#endif /* defined(__BabyHue__FileLogWriter__) */
