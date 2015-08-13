//
//  WebCalls.cpp
//
//  Created by Coen Bresser on 30/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include "WebCalls.h"
#include "Logger.h"

String WebCalls_::doGet(const char *url, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Logger.trace("Execute GET: ");
    Logger.trace(url);
    
    while (_p.running()) {
        delay(100);
    }
    
    if (Logger.getLogLevel() >= LOG_LEVEL_DEBUG) {
        _p.begin("/usr/etc/./lcurl");
    } else {
        _p.begin("curl");
    }
    _p.addParameter("-L");
    _p.addParameter("-k");
    _p.addParameter(url);
    int returnCode = _p.run();
    if (returnCode > 0) {
        Logger.error("CURL error: ", String(returnCode).c_str());
        
        if (errorCallback) {
            errorCallback(_p, returnCode);
        }
        
        _p.flush();
        _p.close();
        return "";
    }
    String t = callback(_p);
    _p.flush();
    _p.close();
    return t;
}

String WebCalls_::doPut(const char *url, const char *data, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Logger.trace("Execute PUT: ");
    Logger.trace(url);
    Logger.trace(data);
    
    while (_p.running()) {
        delay(100);
    }

    if (Logger.getLogLevel() >= LOG_LEVEL_DEBUG) {
        _p.begin("/usr/etc/./lcurl");
    } else {
        _p.begin("curl");
    }
    _p.addParameter("-H");
    _p.addParameter("\"Content-Type: application/json\"");
    _p.addParameter("-X");
    _p.addParameter("PUT");
    _p.addParameter("-d");
    _p.addParameter(data);
    _p.addParameter(url);
    int returnCode = _p.run();
    if (returnCode > 0) {
        Logger.error("CURL error: ", String(returnCode).c_str());
        Logger.error(url);
        Logger.error(data);
        
        if (errorCallback) {
            errorCallback(_p, returnCode);
        }
        
        _p.flush();
        _p.close();
        return "";
    }
    String t = callback ? callback(_p) : "";
    _p.flush();
    _p.close();    
    return t;
}

String WebCalls_::doPost(const char *url, const char *data, WebCallbackType callback, WebErrorCallbackType errorCallback) {
    Logger.trace("Execute POST: ");
    Logger.trace(url);
    Logger.trace(data);
    
    while (_p.running()) {
        delay(100);
    }
    
    if (Logger.getLogLevel() >= LOG_LEVEL_DEBUG) {
        _p.begin("/usr/etc/./lcurl");
    } else {
        _p.begin("curl");
    }
    _p.addParameter("-H");
    _p.addParameter("\"Content-Type: application/json\"");
    _p.addParameter("-X");
    _p.addParameter("POST");
    _p.addParameter("-d");
    _p.addParameter(data);
    _p.addParameter(url);
    int returnCode = _p.run();
    if (returnCode > 0) {
        Logger.error("CURL error: ", String(returnCode).c_str());
        Logger.error(url);
        Logger.error(data);
        
        if (errorCallback) {
            errorCallback(_p, returnCode);
        }
        
        _p.flush();
        _p.close();
        return "";
    }
    String t = callback ? callback(_p) : "";
    _p.flush();
    _p.close();
    return t;
}

//TODO: write the doDelete method

WebCalls_ WebCalls;
