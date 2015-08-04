//
//  FileLogWriter.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "FileLogWriter.h"

File FileLogWriter::getFile() {
    File f = FileSystem.open(logFileName, FILE_APPEND);
    return f;
}

size_t FileLogWriter::write(const char str[]) {
    if (File f = getFile()) {
        int count = f.print(str);
        f.close();
        return count;
    } else {
        return 0;
    }
}

size_t FileLogWriter::write(char c) {
    if (File f = getFile()) {
        int count = f.print(c);
        f.close();
        return count;
    } else {
        return 0;
    }
}

size_t FileLogWriter::writeln(const char c[]) {
    if (File f = getFile()) {
        int count = f.print(c);
        count += f.print("\r\n");
        f.close();
        return count;
    } else {
        return 0;
    }
}

size_t FileLogWriter::writeln(const String &s) {
    if (File f = getFile()) {
        int count = f.print(s);
        count += f.print("\r\n");
        f.close();
        return count;
    } else {
        return 0;
    }
}

void FileLogWriter::flush() { }