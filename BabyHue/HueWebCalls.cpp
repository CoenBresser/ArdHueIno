//
//  HueWebCalls.cpp
//  BabyHue
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "HueWebCalls.h"

String HueWebCalls::doGet(String url, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Serial.println("Execute GET on: " + url);
    Process p;
    p.begin("curl");
    p.addParameter("-L");
    p.addParameter("-k");
    p.addParameter(url);
    int returnCode = p.run();
    if (returnCode > 0) {
        Serial.print("CURL error: ");
        Serial.println(returnCode);
        Serial.flush();
        
        if (errorCallback) {
            errorCallback(p, returnCode);
        }
        
        p.flush();
        p.close();
        return "";
    }
    String t = callback(p);
    p.flush();
    p.close();
    return t;
}

String HueWebCalls::doPut(String url, String data, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Serial.println("Execute PUT on: " + url + ", with data: " + data);
    Process p;
    p.begin("curl");
    p.addParameter("-H");
    p.addParameter("\"Content-Type: application/json\"");
    p.addParameter("-X");
    p.addParameter("PUT");
    p.addParameter("-d");
    p.addParameter(data);
    p.addParameter(url);
    int returnCode = p.run();
    if (returnCode > 0) {
        Serial.print("CURL error: ");
        Serial.println(returnCode);
        Serial.flush();
        
        if (errorCallback) {
            errorCallback(p, returnCode);
        }
        
        p.flush();
        p.close();
        return "";
    }
    String t = callback(p);
    p.flush();
    p.close();    
    return t;
}

String HueWebCalls::doPost(String url, String data, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Serial.println("Execute POST on: " + url + ", with data: " + data);
    Process p;
    p.begin("curl");
    p.addParameter("-H");
    p.addParameter("\"Content-Type: application/json\"");
    p.addParameter("-X");
    p.addParameter("POST");
    p.addParameter("-d");
    p.addParameter(data);
    p.addParameter(url);
    int returnCode = p.run();
    if (returnCode > 0) {
        Serial.print("CURL error: ");
        Serial.println(returnCode);
        Serial.flush();
        
        if (errorCallback) {
            errorCallback(p, returnCode);
        }
        
        p.flush();
        p.close();
        return "";
    }
    String t = callback(p);
    p.flush();
    p.close();
    return t;
}

//TODO: Delete
