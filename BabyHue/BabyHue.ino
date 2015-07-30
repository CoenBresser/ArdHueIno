//#include <aJSON.h>

#include <Bridge.h>
//#include <Console.h>
//#include <FileIO.h>
//#include <HttpClient.h>
//#include <Mailbox.h>
#include <Process.h>
//#include <YunClient.h>
//#include <YunServer.h>

#include <EEPROM.h>

#include "HueWebCalls.h"

int error = 0;        // global error codes to enable visual feedback

int led = 13;           // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// For some reason passing this as a parameter results in not being able to read the stream...
Process p;
HueWebCalls w;

/***** Private methods section *****/

void fadeForMillis(unsigned int totalTime, int delayTime) {
    for (int i = 0; i <= totalTime / delayTime; i++) {
        // set the brightness of the LED pin:
        analogWrite(led, brightness);
        
        // change the brightness for next time through the loop:
        brightness = brightness + fadeAmount;
        
        // reverse the direction of the fading at the ends of the fade:
        if (brightness == 0 || brightness == 255) {
            fadeAmount = -fadeAmount;
        }
        // wait to see the dimming effect
        delay(delayTime);
    }
    analogWrite(led, 0);
}

// EEPROM Start address to read from
// TODO: make this a setting when creating a class
int configAddress = 0;
// Struct to hold the settings
struct Config {
    char userName[33];
};
Config config = { "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" };

String parseHueLocationAnswer(Process p) {
    // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
    // Ignore errors, this will be caught when trying to resolve
    for (int i = 0; i < 7; i++) {
        String dump = p.readStringUntil('"');
    }
    
    // Get the address
    return "http://" + p.readStringUntil('"') + "/api/";
}

String getHueRL() {
    String hueRL = w.doGet("http://www.meethue.com/api/nupnp", [] (Process p) -> String {
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

int lightsUrlLen(String apiBase, String userName) {
    // Note that the closing / for the user is in the lights url
    return apiBase.length() + userName.length() + strlen("/lights/");
}

String buildLightsUrl(String apiBase, String userName) {
    // Note that the closing / for the user is in the lights url
    return apiBase + userName + "/lights/";
}

int alertUrlLen(String apiBase, String userName, String id) {
    // Note that the closing / for id is put in state
    return lightsUrlLen(apiBase, userName) + id.length() + strlen("/state/");
}

String buildAlertUrl(String apiBase, String userName, String id) {
    return buildLightsUrl(apiBase, userName) + id + "/state/";
}

int parseError(Process p) {
    // Set the error to a generic error
    int returnCode = 1;
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        if (subpart == "type") {
            returnCode = p.parseInt();
            Serial.print("Error code: ");
            Serial.println(returnCode);
        }
        if (subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            Serial.println(p.readStringUntil('"'));
        }
    }
    Serial.flush();
    p.flush();
    p.close();
    return returnCode;
}

String parseLightsResponse(Process p) {
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
    p.flush();
    p.close();
    return "success";
}

/**
 * Get the lights configuration
 */
String getLightsConfig(String hueRL, String userName) {
    return w.doGet(buildLightsUrl(hueRL, userName), parseLightsResponse);
}

String parseUserCreateResponse(Process p) {
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
            Serial.print("Error code: ");
            Serial.println(returnCode);
            output += ": " + returnCode;
        }
        if (!isValid && subpart == "description") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            Serial.println(p.readStringUntil('"'));
        }
        if (subpart == "success") {
            isValid = true;
        }
        if (subpart == "username") {
            // Skip the colon - ": "
            p.readStringUntil('"');
            output = p.readStringUntil('"');
            Serial.println("New userName: " + output);
        }
    }
    Serial.flush();
    p.flush();
    p.close();
    return output;
}

/**
 * Do a new user registration
 *
 * Note that this method doesn't exit until there is a registered user
 */
String doNewUserRegistration(String hueRL) {
    while (true) {
        String response = w.doPost(hueRL, "{\"devicetype\":\"Arduino#BabyHue\"}", parseUserCreateResponse);
        
        if (response.startsWith("error") || response == "") {
            fadeForMillis(2000, 10);
            continue;
        }
        
        return response;
    }
    // Should not happen... Generic error
    Serial.println("\r\n!!! Impossible state !!!");
    return "";
}

String getUser(String hueRL) {
    /*
     16 byte hex / 32 nibble hex, just take it as a string
     the 33'rd char holds 0 for the string terminator
     char userName[33] = "27787d893751a7726400ccd3c6db19b";
    */
    
    // Get the user from the EEPROM
    EEPROM.get(configAddress, config);
    
    // Check if we have a valid user by probing the first character and testing it on the Hue
    if (config.userName[0] > 0) {
        
        String resp = getLightsConfig(hueRL, String(config.userName));
        
        // Everything else means there is a problem and we need a new user
        if (resp == "success") {
            return config.userName;
        }
    }
    
    Serial.println("No (valid) user, setting up a new one");
    Serial.flush();
    
    // This method blocks until we have a user or an insolvable error
    String newHueser = doNewUserRegistration(hueRL);
    if (newHueser == "") {
        return "";
    }
    
    Serial.print("Received user: " + newHueser);
    Serial.flush();

    newHueser.toCharArray(config.userName, sizeof(config.userName));
    EEPROM.put(configAddress, config);
    
    // Get the lights configuration. If it fails there's nothing to do anymore
    if (getLightsConfig(hueRL, config.userName) != "success") {
        return "";
    }
    
    // All done
    return String(config.userName);
}

String getLightIdFor(String lightName) {
    // TODO: Get from Brigde
    return "1";
}

String dumpResponseCallback(Process p) {
    while(p.available()) {
        char c = p.read();
        Serial.print(c);
    }
    Serial.println("");
    Serial.flush();
    return "";
}

void enableAlert(String hueRL, String hueser, String lightId, bool once = false) {
    Serial.println("Enabling alert");
    String url = buildAlertUrl(hueRL, hueser, lightId);
    w.doPut(url, once ? "{\"alert\":\"select\"}" : "{\"alert\":\"lselect\"}", dumpResponseCallback);
}

void disableAlert(String hueRL, String hueser, String lightId) {
    Serial.println("Disabling alert");
    String url = buildAlertUrl(hueRL, hueser, lightId);
    w.doPut(url, "{\"alert\":\"none\"}", dumpResponseCallback);
}

void setupHue() {
    Serial.println("Setup Hue");
    
    String hueRL = "";
    while (hueRL == "") {
        hueRL = getHueRL();
        if (hueRL == "") {
            fadeForMillis(10000, 10);
        }
    }
     
    Serial.print("Using Hue on: ");
    Serial.println(hueRL);
    
    String hueser = getUser(hueRL);
    Serial.print("Using username: ");
    Serial.println(hueser);
    Serial.flush();
    if (hueser == "") {
        return;
    }
    
    // We have a valid Hue configuration now
    Serial.println("Manipulating lights now");
    
    // Enable alert on the light called "Color Light"
    enableAlert(hueRL, hueser, getLightIdFor("Color Light"));
    fadeForMillis(10000, 40);
    disableAlert(hueRL, hueser, getLightIdFor("Color Light"));
    
    Serial.flush();
}

/***** Public methods section *****/

void setup() {
    pinMode(led, OUTPUT);
    
    // Initialize Bridge
    Bridge.begin();
    
    // Initialize Serial
    Serial.begin(9600);
    
    // Wait until a Serial Monitor is connected.
    while (!Serial) {
        fadeForMillis(1000, 3);
    }
    
    // Do the hue setup
    setupHue();
}

void loop() {
    fadeForMillis(5000, error ? 20 : 5);
}
