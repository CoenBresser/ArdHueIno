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

/* Examples
 String cbk(Process p) {
     Serial.println("In callback");
     while (p.available()) {
         char c = p.read();
         Serial.print(c);
     }
     return "success";
 }
 
 void go() {
     String error = "";
     if (w.doGet(url, cbk) == "") {
         return error;
     };
     if (error = w.doGet(url, [](Process p) -> String {return 1;})) {
         return "";
     };
 }
*/


#include <Process.h>

typedef String (*WebCallbackType)(Process);
typedef void (*WebErrorCallbackType)(Process p, int exitValue);

class HueWebCalls {
public:
    HueWebCalls() : started(true) { }
    
    String doGet(String url, WebCallbackType callback, WebErrorCallbackType errorCallback = NULL);
    String doPut(String url, String data, WebCallbackType callback, WebErrorCallbackType errorCallback = NULL);
    String doPost(String url, String data, WebCallbackType callback, WebErrorCallbackType errorCallback = NULL);
    String doDelete(String url, WebCallbackType callback, WebErrorCallbackType errorCallback = NULL);
    
private:
    boolean started;
    
};

#endif /* defined(__BabyHue__HueWebCalls__) */
