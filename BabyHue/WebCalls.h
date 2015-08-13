//
//  HueWebCalls.h
//  BabyHue
//
//  Methods to perform Hue web calls
//  Note that these methods return "" upon a failure and the callback is not called in that case
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__HueWebCalls__
#define __BabyHue__HueWebCalls__

#include <Process.h>

typedef String (*WebCallbackType)(Stream&);
typedef void (*WebErrorCallbackType)(Stream&, int);

class WebCalls_ {
public:
    WebCalls_() { }
    
    String doGet(const char *url, WebCallbackType callback, WebErrorCallbackType errorCallback = NULL);
    String doPut(const char *url, const char *data, WebCallbackType callback = NULL, WebErrorCallbackType errorCallback = NULL);
    String doPost(const char *url, const char *data, WebCallbackType callback = NULL, WebErrorCallbackType errorCallback = NULL);
    String doDelete(const char *url, WebCallbackType callback = NULL, WebErrorCallbackType errorCallback = NULL);
    
private:
    Process _p;
};
extern WebCalls_ WebCalls;

#endif /* defined(__BabyHue__HueWebCalls__) */
