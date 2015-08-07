//
//  CharStream.h
//  BabyHue
//
//  Created by Coen Bresser on 05/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__CharStream__
#define __BabyHue__CharStream__

#include <Stream.h>

class CharStream : Stream {
public:
    CharStream(const char *pCharArray, int size) : pCharArray(pCharArray), size(size) { };
    
    int available();
    int read();
    int peek();
    void flush();

private:
    const char *pCharArray;
    const int size;
    int current = 0;
};

#endif /* defined(__BabyHue__CharStream__) */
