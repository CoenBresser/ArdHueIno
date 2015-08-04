//
//  TwoLogWriters.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "TwoLogWriters.h"

size_t TwoLogWriters::write(const char str[]) {
    first->write(str);
    return second->write(str);
}

size_t TwoLogWriters::write(char c) {
    first->write(c);
    return second->write(c);
}

size_t TwoLogWriters::writeln(const char c[]) {
    first->writeln(c);
    return second->writeln(c);
}

size_t TwoLogWriters::writeln(const String &s) {
    first->writeln(s);
    return second->writeln(s);
}

void TwoLogWriters::flush() {
    first->flush();
    second->flush();
}
