//
//  Hue.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

//TODO: Rewrite to asynchrounous

#include <Bridge.h>
#include "Hue.h"
#include "WebCalls.h"
#include "Logger.h"
#include <EEPROM.h>
#include <stdarg.h>

#define BRIDGE_LIGHT_STATE_FORMAT "on:%s,bri:%d,hue:%d,sat:%d,"

#define HUE_GROUP_NAME "BabyHue"
#define HUE_GROUP_BRIDGE_ID "BabyHueGroupId"

// Struct to hold the settings
struct HueConfig {
    char userName[33];
};
HueConfig hueConfig = {
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
};

void Hue_::begin(void (*waitFunction)(void)) {
    
    Logger.info("Setting up Hue on Yun");
    
    Process _hueP;
    _hueP.begin("/usr/BabyHue/./doStartYunHue.sh");
    _hueP.runAsynchronously();
    while (_hueP.running()) {
        // TODO: check if we can receive data while running, this would make things more flexible!
        if (waitFunction) {
            waitFunction();
        } else {
            delay(100);
        }
    }
    Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG); // Do something with the received OK
    _hueP.flush();
    _hueP.close();

    
    /*
    // Get the config from the EEPROM
    EEPROM.get(eepromBaseAddress, hueConfig);
    
    while (strlen(hueRL) == 0) {
        getHueRL();
        if (strlen(hueRL) == 0) {
            if (waitFunction) {
                waitFunction();
            } else {
                delay(1000);
            }
        }
    }
    
    Logger.info("Using Hue on: ", hueRL);
    
    doGetValidateHueser(waitFunction);
    Logger.info("Using username: ", hueser);
    
    if (hueser == "") {
        return;
    }

    Logger.info("Checking group config");
    checkCreateHueGroup(waitFunction);
     */
}

/************************************** Builders *************************************/

// For memory purposes use a single String buffer. Note that with concurrent access this is not valid!
char strBuf[75];
const char* buildString(int n, ...){
    va_list vl;
    va_start(vl, n);
    
    int len = 0;
    for (int i = 0; i < n; i++) {
        char *val = va_arg(vl, char*);

        if (len + strlen(val) > sizeof(strBuf)) {
            Logger.error("Trying to create string longer than available buffer");
            break;
        }
        len += strlen(val);
        if (i == 0) {
            strcpy(strBuf, val);
        } else {
            strcat(strBuf, val);
        }
    }
    
    va_end(vl);
    
    Logger.trace("Built string: ", strBuf);
    return strBuf;
}

const char* Hue_::buildLightsBaseUrl() {
    return buildString(3, hueRL, hueser, "/lights/");
}

const char* Hue_::buildLightStatePutUrl(int id) {
    return buildString(5, hueRL, hueser, "/lights/", String(id).c_str(), "/state/");
}

const char* Hue_::buildLightGetUrl(int id) {
    return buildString(5, hueRL, hueser, "/lights/", String(id).c_str(), "/");
}

const char* Hue_::buildGroupsBaseUrl() {
    return buildString(3, hueRL, hueser, "/groups/");
}

const char* Hue_::buildGroupUrl() {
    char id[3];
    
    Bridge.get(HUE_GROUP_BRIDGE_ID, id, sizeof(id));
    
    Logger.info("Using group id: ", id);
    
    return buildString(5, hueRL, hueser, "/groups/", id, "/");
}

const char* Hue_::buildGroupActionUrl() {
    char id[3];
    
    Bridge.get(HUE_GROUP_BRIDGE_ID, id, sizeof(id));
    
    Logger.info("Using group id: ", id);

    return buildString(5, hueRL, hueser, "/groups/", id, "/action/");
}

/************************************** Parsers **************************************/

// Note, this is a stranger, it is used as a submethod in below generic parsers
int parseError(Stream& p) {
    // Set the error to a generic error
    int returnCode = 1;
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        if (subpart == "type") {
            returnCode = p.parseInt();
            Logger.error("Error code: " + returnCode);
        }
        if (subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            String s = p.readStringUntil('"');
            Logger.error(s);
        }
    }
    return returnCode;
}

String dumpResponseCallback(Stream& p) {
    Logger.dumpStream(p, LOG_LEVEL_DUMP);
    return "";
}

String parseHueLocationAnswer(Stream& p) {
    // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
    // Ignore errors, this will be caught when trying to resolve
    for (int i = 0; i < 7; i++) {
        String dump = p.readStringUntil('"');
    }
    
    // Get the address
    return "http://" + p.readStringUntil('"') + "/api/";
}

String parseLightsResponse(Stream& p) {
    int level = 0;
    int numLights = 0;
    String currentObj = "root";
    
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        
        /**** 
         **** Warning: Below lines work because the Hue returns the level based on the
         **** amount of }}'s with or without a space, see the actual JSON over the line
         ****/
        if (subpart.indexOf('{') >= 0) {
            level++;
        }
        if (subpart.indexOf('}') >= 0) {
            level--;
        }
        if (subpart.indexOf("}}") >= 0) {
            level--;
        }
        if (subpart.indexOf("}}}") >= 0) {
            level--;
        }
        /****/
        
        if (subpart == "error") {
            return String(parseError(p));
        }
        
        if (level == 1 && subpart.toInt() > 0) {
            numLights++;
            // Store for later use
        }
        if (level == 2 && subpart == "state") {
            currentObj = "state";
        }
        if (level == 3 && currentObj == "state" && subpart == "reachable") {
            // Check if available
            p.readStringUntil(':');
            char boolFirstChar = p.read();
            Bridge.put("light" + String(numLights) + "Available", boolFirstChar == 't' ? "true" : "false");
        }
        if (level == 2 && subpart == "type") {
            // Get the type
            p.readStringUntil('"');
            Bridge.put("light" + String(numLights) + "Type", p.readStringUntil('"'));
        }
        if (level == 2 && subpart == "name") {
            // Get the name
            p.readStringUntil('"');
            Bridge.put("light" + String(numLights) + "Name", p.readStringUntil('"'));
        }
    }
    Bridge.put("nrOfLights", String(numLights));
    return "success";
}

String parseUserCreateResponse(Stream& p) {
    // Examples:
    //[{"success":{"username":"27787d893751a7726400ccd3c6db19b"}}]
    //[{"error":{"type":101,"address":"/","description":"link button not pressed"}}]
    
    String output;
    bool isValid = true;
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        if (subpart == "error") {
            isValid = false;
            output = "error";
        }
        if (!isValid && subpart == "type") {
            int returnCode = p.parseInt();
            Logger.error("Error code: " + returnCode);
            output += ": " + returnCode;
        }
        if (!isValid && subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            String s = p.readStringUntil('"');
            Logger.error(s);
        }
        if (subpart == "success") {
            isValid = true;
        }
        if (subpart == "username") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            output = p.readStringUntil('"');
            Logger.info("New userName: " + output);
        }
    }
    return output;
}

String parseLightState(Stream& s) {
    Logger.trace("parseLightState");
    
    // WARNING: When the hue order of elements changes the fixed format string BRIDGE_LIGHT_STATE_FORMAT will be wrong
    // If that's the case, fix it here so in the arduino we can use a fixed format again
    String state = "";
    int bri, hue, sat;
    while (s.available()) {
        String part = s.readStringUntil('"');
        
        if (part == "on") {
            state += part + s.readStringUntil(',') + ",";
        } else if (part == "bri") {
            state += part + s.readStringUntil(',') + ",";
            
        } else if (part == "sat") {
            state += part + s.readStringUntil(',') + ",";
            
        } else if (part == "hue") {
            state += part + s.readStringUntil(',') + ",";
        }
    }
    return state;
}

/* Examples, well formatted. The actual response is minimal (no tabs, nog spaces)
// No Groups
{}

// 1 (correct) group
{
    "1": {
        "name": "BabyHue",
        "lights": [
                   "1",
                   "3",
                   "4"
                   ],
        "type": "LightGroup",
        "action": {
            "on": false,
            "bri": 202,
            "hue": 13277,
            "sat": 207,
            "effect": "none",
            "xy": [
                   0.508,
                   0.415
                   ],
            "ct": 459,
            "alert": "none",
            "colormode": "hs"
        }
    }
}

// 2 groups
{
    "1": {
        "name": "BabyHue",
        "lights": [
                   "1",
                   "3",
                   "4"
                   ],
        "type": "LightGroup",
        "action": {
            "on": false,
            "bri": 202,
            "hue": 13277,
            "sat": 207,
            "effect": "none",
            "xy": [
                   0.508,
                   0.415
                   ],
            "ct": 459,
            "alert": "none",
            "colormode": "hs"
        }
    },
    "2": {
        "name": "bedroom",
        "lights": [
                   "1",
                   "2"
                   ],
        "type": "LightGroup",
        "action": {
            "on": false,
            "bri": 0,
            "hue": 25760,
            "sat": 254,
            "effect": "none",
            "xy": [
                   0.2147,
                   0.7079
                   ],
            "alert": "none",
            "colormode": "hs"
        }
    }
}
*/

String parseHueGroupForId(Stream& s) {
    
    String id = "";
    while (s.available()) {
        String part = s.readStringUntil('"');
        
        // No groups
        if (part == "{}") {
            break;
        }
        
        if (id == "") {
            // First time, easy
            id = s.readStringUntil('"');
            continue;
        }
        if (part == "}},") {
            // This just precedes the id. Note that this is very weak
            Logger.debug("Using weak determination of id for groups");
            id = s.readStringUntil('"');
            continue;
        }
        
        // Just track the easy way, if we have the correct name, the id is correct
        if (part == HUE_GROUP_NAME) {
            break;
        }
    }
    return id;
}

/************************************** Store/restore ********************************/

void Hue_::storeLightStates() {
    Logger.info("Storing light states");
    
    Process _hueP;
    _hueP.begin("/usr/BabyHue/./doStoreGroupLightsState.sh");
    _hueP.runAsynchronously();
    while (_hueP.running()) {
        delay(100);
    }
    
    // Ignore response in this script
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG);
    
    _hueP.flush();
    _hueP.close();
}

void Hue_::restoreLightStates() {
    Logger.info("Restoring light states");
    
    Process _hueP;
    _hueP.begin("/usr/BabyHue/./doRestoreGroupLightsState.sh");
    _hueP.runAsynchronously();
    while (_hueP.running()) {
        delay(100);
    }
    // Ignore response in this script
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG);
    
    _hueP.flush();
    _hueP.close();
}

/************************************** Ungrouped ************************************/


void Hue_::checkCreateHueGroup(void (*waitFunction)(void)) {
    const char *url = buildGroupsBaseUrl();
    String groupId = WebCalls.doGet(url, parseHueGroupForId);
    Logger.debug("Group id: " + groupId);
    
    while (groupId == "") {
        // Create a group with light 1 in it
        const char *data = "{\"lights\":[\"1\"],\"name\":\"BabyHue\"}";
        groupId = WebCalls.doPost(url, data, [] (Stream& s) -> String {
            Logger.trace("Parsing answer");
            while (s.available()) {
                String part = s.readStringUntil('"');
                Logger.debug(part);
                if (part == "id") {
                    s.readStringUntil('"');
                    String id = s.readStringUntil('"');
                    return id;
                }
            }
        });
        String s = "Parsed group id: " + groupId;
        Logger.debug(s);
        if (groupId == "") {
            Logger.warn("Error in group registration, retry.");
            if (waitFunction) {
                waitFunction();
            } else {
                delay(5000);
            }
        }
    }
    
    // Store the id in the bridge
    Bridge.put(HUE_GROUP_BRIDGE_ID, groupId);
}

String Hue_::doGetLightsConfig() {
    const char *url = buildLightsBaseUrl();
    return WebCalls.doGet(url, parseLightsResponse);
}

/**
 * Do a new user registration
 *
 * Note that this method doesn't exit until there is a registered user
 */
String Hue_::doNewUserRegistration(void (*waitFunction)(void)) {
    Logger.info(hueRL);
    
    while (true) {
        const char *data = "{\"devicetype\":\"Arduino#BabyHue\"}";
        String response = WebCalls.doPost(hueRL, data, parseUserCreateResponse);
        
        if (response.startsWith("error") || response == "") {
            if (waitFunction) {
                waitFunction();
            } else {
                Logger.warn("Press the Hue button for user registration.");
                delay(5000);
            }
            continue;
        }
        
        return response;
    }
    // Should not happen... Generic error
    Logger.fail("!!! Impossible state !!!");
    exit(0);
    return "";
}

void Hue_::doGetValidateHueser(void (*waitFunction)(void)) {
    
    // Check if we have a valid user by probing the first character and testing it on the Hue
    if (hueConfig.userName[0] > 0) {
        
        Logger.info(hueConfig.userName);
        
        strcpy(hueser, hueConfig.userName);
        String resp = doGetLightsConfig();
        
        // Everything else means there is a problem and we need a new user
        if (resp == "success") {
            return;
        }
    }
    
    Logger.info("No (valid) user, setting up a new one");
        
    // This method blocks until we have a user or an insolvable error
    String newHueser = doNewUserRegistration(waitFunction);
    if (newHueser == "") {
        return;
    }
    
    newHueser.toCharArray(hueConfig.userName, sizeof(hueConfig.userName));
    EEPROM.put(eepromBaseAddress, hueConfig);
    
    Logger.info("Registered new user: ", hueConfig.userName);
    strcpy(hueser, hueConfig.userName);
    
    // Get the lights configuration. If it fails there's nothing to do anymore
    if (doGetLightsConfig() != "success") {
        Logger.error("Cannot get lights with new user");
        return;
    }
    
    // All done
    return;
}

void Hue_::enableAlert(int lightId, bool once) {
    Logger.trace("Enabling alert");
    const char *url = buildLightStatePutUrl(lightId);
    const char *data = (once ? "{\"alert\":\"select\"}" : "{\"alert\":\"lselect\"}");
    WebCalls.doPut(url, data, dumpResponseCallback);
}

void Hue_::disableAlert(int lightId) {
    Logger.trace("Disabling alert");
    const char *url = buildLightStatePutUrl(lightId);
    const char *data = "{\"alert\":\"none\"}";
    WebCalls.doPut(url, data, dumpResponseCallback);
}

void Hue_::getHueRL() {
    const char *url = "http://www.meethue.com/api/nupnp";
    String hueRLs = WebCalls.doGet(url, [] (Stream& p) -> String {
        // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
        // Ignore errors, this will be caught when trying to resolve
        for (int i = 0; i < 7; i++) {
            String dump = p.readStringUntil('"');
        }
        
        // Get the address
        return "http://" + p.readStringUntil('"') + "/api/";
    });
    
    hueRLs.toCharArray(hueRL, sizeof(hueRL));
    Bridge.put("HueRL", hueRL);
}

char* buildBridgeLightStateKey(int lightId) {
    String id = "Light" + String(lightId) + "State";
    char buf[id.length()];
    id.toCharArray(buf, id.length());
    return buf;
}

void Hue_::getLightIdsFromGroup() {
    for (int i = 0; i < MAX_HUE_GROUP_MEMBERS; i++) {
        lightIds[i] = 0;
    }
    const char* url = buildGroupUrl();
    String lights = WebCalls.doGet(url, [] (Stream& s) -> String {
        while (s.available()) {
            String part = s.readStringUntil('"');
            if (part == "lights") {
                s.readStringUntil('[');
                return s.readStringUntil(']');
            }
        }
        return "";
    });
    
    if (lights.length() > 0) {
        String buf = "";
        int id = 0;
        for (int i = 0; i < lights.length(); i++) {
            if (lights.charAt(i) == '"') {
                continue;
            }
            if (lights.charAt(i) == ',') {
                lightIds[id++] = buf.toInt();
                buf = "";
                continue;
            }
            buf += lights.charAt(i);
        }
        lightIds[id] = buf.toInt();
    }
    
    for (int i = 0; i < sizeof(lightIds) && lightIds[i] > 0; i++) {
        char buf[20];
        sprintf(buf, "Light %d id: %d", i, lightIds[i]);
        Logger.trace(buf);
    }
}

void Hue_::setLightState(int lightId, bool on, int brightness, int hue, int saturation) {
    const char *url = "http://192.168.2.2/api/1333e79533fe760714fc846116f5a333/lights/1/state/"; //buildLightStatePutUrl(lightId);
    
    // (set to off is the last action, set to on the first)
    char data[55];
    if (on) {
        sprintf(data, "{\"on\": %s, \"bri\": %d, \"hue\": %d, \"sat\": %d}", on ? "true" : "false", brightness, hue, saturation);
    } else {
        sprintf(data, "{\"bri\": %d, \"hue\": %d, \"sat\": %d, \"on\": %s}", brightness, hue, saturation, on ? "true" : "false");
    }
    
    WebCalls.doPut(url, data);
}

void Hue_::setLightStates(bool on, int brightness, int hue, int saturation) {
    const char *url = buildGroupActionUrl();
    
    // (set to off is the last action, set to on the first)
    char data[55];
    if (on) {
        sprintf(data, "{\"on\": %s, \"bri\": %d, \"hue\": %d, \"sat\": %d}", on ? "true" : "false", brightness, hue, saturation);
    } else {
        sprintf(data, "{\"bri\": %d, \"hue\": %d, \"sat\": %d, \"on\": %s}", brightness, hue, saturation, on ? "true" : "false");
    }
    
    WebCalls.doPut(url, data);
}

void Hue_::revealSelectedLights(void (*waitFunction)(void)) {
    Logger.info("Revealing lights in group");
    
    Process _hueP;
    _hueP.begin("/usr/BabyHue/./doNotifyGroup.sh");
    _hueP.runAsynchronously();
    while (_hueP.running()) {
        if (waitFunction) {
            waitFunction();
        } else {
            delay(100);
        }
    }
    
    // No response in this script
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG);
    
    _hueP.flush();
    _hueP.close();
}

Hue_ Hue;
