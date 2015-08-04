//
//  Hue.cpp
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include <Bridge.h>
#include "Hue.h"
#include "HueWebCalls.h"
#include "LedControl.h"
#include "Logger.h"
#include <EEPROM.h>

extern Logger logger;
Process p;
HueWebCalls w;

// Struct to hold the settings
struct Config {
    char userName[33];
};
Config config = { "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" };

void Hue::begin() {
    while (hueRL == "") {
        hueRL = getHueRL();
        if (hueRL == "") {
            LedControl::fadeForMillis(10000, 10);
        }
    }
    
    logger.info("Using Hue on: " + hueRL);
    
    String hueser = doGetValidateHueser();
    logger.info("Using username: " + hueser);
    
    if (hueser == "") {
        return;
    }

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

String Hue::buildLightsBaseUrl() {
    // Note that the closing / for the user is in the lights url
    return hueRL + hueser + "/lights/";
}

String Hue::buildAlertUrl(int id) {
    return buildLightsBaseUrl() + id + "/state/";
}

int parseError(Stream& p) {
    // Set the error to a generic error
    int returnCode = 1;
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        if (subpart == "type") {
            returnCode = p.parseInt();
            logger.error("Error code: " + returnCode);
        }
        if (subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            logger.error(p.readStringUntil('"'));
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
String Hue::doGetLightsConfig() {
    return w.doGet(buildLightsBaseUrl(), parseLightsResponse);
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
            logger.error("Error code: " + returnCode);
            output += ": " + returnCode;
        }
        if (!isValid && subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            logger.error(p.readStringUntil('"'));
        }
        if (subpart == "success") {
            isValid = true;
        }
        if (subpart == "username") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            output = p.readStringUntil('"');
            logger.info("New userName: " + output);
        }
    }
    return output;
}

/**
 * Do a new user registration
 *
 * Note that this method doesn't exit until there is a registered user
 */
String Hue::doNewUserRegistration() {
    while (true) {
        String response = w.doPost(hueRL, "{\"devicetype\":\"Arduino#BabyHue\"}", parseUserCreateResponse);
        
        if (response.startsWith("error") || response == "") {
            LedControl::fadeForMillis(2000, 10);
            continue;
        }
        
        return response;
    }
    // Should not happen... Generic error
    logger.fail("!!! Impossible state !!!");
    exit(0);
    return "";
}

String Hue::doGetValidateHueser() {
    /*
     16 byte hex / 32 nibble hex, just take it as a string
     the 33'rd char holds 0 for the string terminator
     char userName[33] = "27787d893751a7726400ccd3c6db19b";
     */
    
    // Get the user from the EEPROM
    EEPROM.get(eepromBaseAddress, config);
    
    // Check if we have a valid user by probing the first character and testing it on the Hue
    if (config.userName[0] > 0) {
        
        hueser = String(config.userName);
        String resp = doGetLightsConfig();
        
        // Everything else means there is a problem and we need a new user
        if (resp == "success") {
            return config.userName;
        }
    }
    
    logger.info("No (valid) user, setting up a new one");
    
    
    // This method blocks until we have a user or an insolvable error
    String newHueser = doNewUserRegistration();
    if (newHueser == "") {
        return "";
    }
    
    logger.info("Registered new user: " + newHueser);
    
    newHueser.toCharArray(config.userName, sizeof(config.userName));
    EEPROM.put(eepromBaseAddress, config);
    
    // Get the lights configuration. If it fails there's nothing to do anymore
    if (doGetLightsConfig() != "success") {
        return "";
    }
    
    // All done
    return String(config.userName);
}

int getLightIdFor(String lightName) {
    // TODO: Get from Brigde
    return 1;
}

String dumpResponseCallback(Stream& p) {
    logger.dumpStream(p, Logger::Trace);
    return "";
}

void Hue::enableAlert(String lightName, bool once) {
    enableAlert(getLightIdFor(lightName), once);
}

void Hue::enableAlert(int lightId, bool once) {
    logger.trace("Enabling alert");
    String url = buildAlertUrl(lightId);
    w.doPut(url, once ? "{\"alert\":\"select\"}" : "{\"alert\":\"lselect\"}", dumpResponseCallback);
}

void Hue::disableAlert(String lightName) {
    disableAlert(getLightIdFor(lightName));
}

void Hue::disableAlert(int lightId) {
    logger.trace("Disabling alert");
    String url = buildAlertUrl(lightId);
    w.doPut(url, "{\"alert\":\"none\"}", dumpResponseCallback);
}

String Hue::getHueRL() {
    String hueRL = w.doGet("http://www.meethue.com/api/nupnp", [] (Stream& p) -> String {
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
