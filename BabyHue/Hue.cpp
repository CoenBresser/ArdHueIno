//
//  Hue.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include <Bridge.h>
#include "Hue.h"
#include "WebCalls.h"
#include "Logger.h"
#include <EEPROM.h>

Process p;

// Struct to hold the settings
struct HueConfig {
    char userName[33];
    int lightIds[10];
};
HueConfig hueConfig = {
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    {1 /* Color Light */, 3 /* Trap omhoog */, 4 /* Grote barbapappa */, 0, 0, 0, 0, 0, 0, 0}
};

void Hue_::begin(void (*waitFunction)(void)) {
    
    // Get the config from the EEPROM
    EEPROM.get(eepromBaseAddress, hueConfig);

    if (hueConfig.lightIds[0] == 0) {
        Logger.warn("Updating config");
        hueConfig.lightIds[0] = 1;
        hueConfig.lightIds[1] = 3;
        hueConfig.lightIds[2] = 4;
        EEPROM.put(eepromBaseAddress, hueConfig);
    }
    
    
    //
    while (hueRL == "") {
        hueRL = getHueRL();
        if (hueRL == "") {
            if (waitFunction) {
                waitFunction();
            } else {
                delay(1000);
            }
        }
    }
    
    Logger.info("Using Hue on: " + hueRL);
    
    String hueser = doGetValidateHueser(waitFunction);
    Logger.info("Using username: " + hueser);
    
    if (hueser == "") {
        return;
    }

    Logger.info("Checking group config");
    checkCreateHueGroup();
}

Hue_::checkCreateHueGroup() {
    
}

String parseHueLocationAnswer(Process p) {
    // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
    // Ignore errors, this will be caught when trying to resolve
    for (int i = 0; i < 7; i++) {
        String dump = p.readStringUntil('"');
    }
    
    // Get the address
    return "http://" + p.readStringUntil('"') + "/api/";
}

String Hue_::buildLightsBaseUrl() {
    // Note that the closing / for the user is in the lights url
    return hueRL + hueser + "/lights/";
}

String Hue_::buildLightStatePutUrl(int id) {
    return buildLightsBaseUrl() + id + "/state/";
}

String Hue_::buildLightGetUrl(int id) {
    return buildLightsBaseUrl() + id + "/";
}

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

String parseLightsResponse(Stream& p) {
    int level = 0;
    int numLights = 0;
    String currentObj = "root";
    
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
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

/**
 * Get the lights configuration
 */
String Hue_::doGetLightsConfig() {
    String url = buildLightsBaseUrl();
    return WebCalls.doGet(url, parseLightsResponse);
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

/**
 * Do a new user registration
 *
 * Note that this method doesn't exit until there is a registered user
 */
String Hue_::doNewUserRegistration(void (*waitFunction)(void)) {
    while (true) {
        String data = "{\"devicetype\":\"Arduino#BabyHue\"}";
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

String Hue_::doGetValidateHueser(void (*waitFunction)(void)) {
    /*
     16 byte hex / 32 nibble hex, just take it as a string
     the 33'rd char holds 0 for the string terminator
     char userName[33] = "27787d893751a7726400ccd3c6db19b";
     */
    
    // Check if we have a valid user by probing the first character and testing it on the Hue
    if (hueConfig.userName[0] > 0) {
        
        hueser = String(hueConfig.userName);
        String resp = doGetLightsConfig();
        
        // Everything else means there is a problem and we need a new user
        if (resp == "success") {
            return hueConfig.userName;
        }
    }
    
    Logger.info("No (valid) user, setting up a new one");
    
    
    // This method blocks until we have a user or an insolvable error
    String newHueser = doNewUserRegistration(waitFunction);
    if (newHueser == "") {
        return "";
    }
    
    Logger.info("Registered new user: " + newHueser);
    
    newHueser.toCharArray(hueConfig.userName, sizeof(hueConfig.userName));
    EEPROM.put(eepromBaseAddress, hueConfig);
    
    // Get the lights configuration. If it fails there's nothing to do anymore
    if (doGetLightsConfig() != "success") {
        return "";
    }
    
    // All done
    return String(hueConfig.userName);
}

String dumpResponseCallback(Stream& p) {
    Logger.dumpStream(p, LOG_LEVEL_DUMP);
    return "";
}

void Hue_::enableAlert(int lightId, bool once) {
    Logger.trace("Enabling alert");
    String url = buildLightStatePutUrl(lightId);
    String data = (once ? "{\"alert\":\"select\"}" : "{\"alert\":\"lselect\"}");
    WebCalls.doPut(url, data, dumpResponseCallback);
}

void Hue_::disableAlert(int lightId) {
    Logger.trace("Disabling alert");
    String url = buildLightStatePutUrl(lightId);
    String data = "{\"alert\":\"none\"}";
    WebCalls.doPut(url, data, dumpResponseCallback);
}

String Hue_::getHueRL() {
    String url = "http://www.meethue.com/api/nupnp";
    String hueRL = WebCalls.doGet(url, [] (Stream& p) -> String {
        // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
        // Ignore errors, this will be caught when trying to resolve
        for (int i = 0; i < 7; i++) {
            String dump = p.readStringUntil('"');
        }
        
        // Get the address
        return "http://" + p.readStringUntil('"') + "/api/";
    });
    Bridge.put("HueRL", hueRL);
    return hueRL;
}

String parseLightState(Stream& s) {
    String state = "";
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

char* buildBridgeLightStateKey(int lightId) {
    String id = "Light" + String(lightId) + "State";
    char buf[id.length()];
    id.toCharArray(buf, id.length());
    return buf;
}

void Hue_::storeLightState(int lightId) {
    String url = buildLightGetUrl(lightId);
    String state = WebCalls.doGet(url, parseLightState);
    
    Logger.debug("Storing state: " + state);
    Bridge.put("Light" + String(lightId) + "State", state);
}

void Hue_::storeLightStates() {
    for (int i = 0; i < sizeof(hueConfig.lightIds) && hueConfig.lightIds[i] > 0; i++) {
        storeLightState(hueConfig.lightIds[i]);
    }
}

String getValueFromState(String state, String part) {
    int idx = state.indexOf(part);
    return state.substring(state.indexOf(':', idx) + 1, state.indexOf(',', idx));
}

void Hue_::restoreLightState(int lightId) {
    char bridgeState[35]; // max length possible
    
    String key = "Light" + String(lightId) + "State";
    Bridge.get(key.c_str(), bridgeState, sizeof(bridgeState));
    
    String bs = String(bridgeState);
    Logger.debug("Restoring Bridge state: " + bs);
    
    String onS = getValueFromState(bs, "on");
    String briS = getValueFromState(bs, "bri");
    String hueS = getValueFromState(bs, "hue");
    String satS = getValueFromState(bs, "sat");
    
    setLightState(lightId, onS, briS, hueS, satS);
}

void Hue_::restoreLightStates() {
    for (int i = 0; i < sizeof(hueConfig.lightIds) && hueConfig.lightIds[i] > 0; i++) {
        restoreLightState(hueConfig.lightIds[i]);
    }
}

void Hue_::setLightState(int lightId, bool on, int brightness, int hue, int saturation) {
    String onS = on ? "true" : "false";
    String briS = String(brightness);
    String hueS = String(hue);
    String satS = String(saturation);
    
    setLightState(lightId, onS, briS, hueS, satS);
}

// TODO: Fix that setting hue, brightness & saturation is performed when the light is on
// (set to off is the last action, set to on the first)
void Hue_::setLightState(int lightId, String& on, String& brightness, String& hue, String& saturation) {
    String url = buildLightStatePutUrl(lightId);
    String data = "{\"on\": " + on + ", \"bri\": " + brightness + ", \"hue\": " + hue + ", \"sat\": " + saturation + "}";
    WebCalls.doPut(url, data, dumpResponseCallback);
}

void Hue_::setLightStates(bool on, int brightness, int hue, int saturation) {
    for (int i = 0; i < sizeof(hueConfig.lightIds) && hueConfig.lightIds[i] > 0; i++) {
        setLightState(hueConfig.lightIds[i], on, brightness, hue, saturation);
    }
}

void Hue_::revealSelectedLights(void (*waitFunction)(void)) {
    bool once = (waitFunction == NULL);
    
    for (int i = 0; i < sizeof(hueConfig.lightIds) && hueConfig.lightIds[i] > 0; i++) {
        enableAlert(hueConfig.lightIds[i], once);
    }
    
    if (!once) {
        waitFunction();
        for (int i = 0; i < sizeof(hueConfig.lightIds) && hueConfig.lightIds[i] > 0; i++) {
            disableAlert(hueConfig.lightIds[i]);
        }
    }
}

// TODO: groups interaction with group BabyHue i.o. individual lights:
// {"lights":["1","3","4"],"name":"BabyHue"}
// {"on": false,"hue": 0,"bri": 255,"sat": 255}

Hue_ Hue;
