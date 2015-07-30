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

int error = 0;        // global error codes to enable visual feedback

int led = 13;           // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// For some reason passing this as a parameter results in not being able to read the stream...
Process p;

/***** Private methods section *****/

int doGetRequest(String url) {
    Serial.println("Execute GET on: " + url);
    p.begin("curl");
    p.addParameter("-L");
    p.addParameter("-k");
    p.addParameter(url);
    if (error = p.run()) {
        Serial.print("CURL error: ");
        Serial.println(error);
        Serial.flush();
        p.flush();
        p.close();
    }
    return error;
}

int doPostRequest(String url, String data) {
    Serial.println("Execute POST on: " + url + ", with data: " + data);
    p.begin("curl");
    p.addParameter("-H");
    p.addParameter("\"Content-Type: application/json\"");
    p.addParameter("-X");
    p.addParameter("POST");
    p.addParameter("-d");
    p.addParameter(data);
    p.addParameter(url);
    if (error = p.run()) {
        Serial.print("CURL error: ");
        Serial.println(error);
        Serial.flush();
        p.flush();
        p.close();
    }
    return error;
}

int doPutRequest(String url, String data) {
    Serial.println("Execute PUT on: " + url + ", with data: " + data);
    p.begin("curl");
    p.addParameter("-H");
    p.addParameter("\"Content-Type: application/json\"");
    p.addParameter("-X");
    p.addParameter("PUT");
    p.addParameter("-d");
    p.addParameter(data);
    p.addParameter(url);
    if (error = p.run()) {
        Serial.print("CURL error: ");
        Serial.println(error);
        Serial.flush();
        p.flush();
        p.close();
    }
    return error;
}

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

String getHueRL() {
    Serial.println("getHueRL");
    
    if (false) {
        
        if (doGetRequest("http://www.meethue.com/api/nupnp") > 0) {
            Serial.println("error");
            return "";
        }
        
        while (p.available()) {
            char c = p.read();
            Serial.print(c);
        }
        p.flush();
        p.close();
        Serial.flush();
    
        return "";
    }
    
    if (doGetRequest("http://www.meethue.com/api/nupnp") > 0) {
        return "";
    }
    
    Serial.println("Parse hue locator answer");
    Serial.flush();
    
    // Search for the 7th " in the string (e.g. [{"id":"001234556","internalipaddress":"192.168.2.2"}])
    // Ignore errors, this will be caught when trying to resolve
    for (int i = 0; i < 7; i++) {
        String dump = p.readStringUntil('"');
    }
    
    // Get the address
    String hueRL = "http://" + p.readStringUntil('"') + "/api/";
    p.flush();
    p.close();
    
    Bridge.put("HueRL", hueRL);
    
    Serial.println(hueRL);
    Serial.flush();
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
    Serial.println("Parsing error");
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

int checkLightsResponse(Process p) {
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
            return parseError(p);
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
    return 0;
}

/**
 * Get the lights configuration
 */
int getLightsConfig(String hueRL, String userName) {
    //char url[lightsUrlLen(hueRL, userName)];
    String url = buildLightsUrl(hueRL, userName);
    
    doGetRequest(url);
    return checkLightsResponse(p);
}

int checkUserCreateResponse(Process p, char* userNameDest) {
    // Examples:
    //[{"success":{"username":"27787d893751a7726400ccd3c6db19b"}}]
    //[{"error":{"type":101,"address":"/","description":"link button not pressed"}}]
    
    int returnCode = 0;
    bool isValid = true;
    while (p.available() > 0) {
        String subpart = p.readStringUntil('"');
        if (subpart == "error") {
            isValid = false;
        }
        if (!isValid && subpart == "type") {
            returnCode = p.parseInt();
            Serial.print("Error code: ");
            Serial.println(returnCode);
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
            int nrBytesRead = p.readBytesUntil('"', userNameDest, 32);
            userNameDest[nrBytesRead] = 0; // terminate the string
            Serial.print("New userName: ");
            Serial.println(userNameDest);
        }
    }
    Serial.flush();
    p.flush();
    p.close();
    return returnCode;
}

/**
 * Do a new user registration
 */
int doNewUserRegistration(String hueRL, char* userNameDest) {
    while (true) {
        if (error = doPostRequest(hueRL, "{\"devicetype\":\"Arduino#BabyHue\"}")) {
            return error;
        };
        
        switch (int returncode = checkUserCreateResponse(p, userNameDest)) {
            case 0: // success
                return 0;
            case 101: // link button not pressed, this is mentioned in the error receiver
                fadeForMillis(2000, 10);
                continue;
            default: // Some kind of error
                error = returncode;
                Serial.print("Unknown registration error: ");
                Serial.println(error);
                return returncode;
        }
    }
    // Should not happen... Generic error
    Serial.println("\r\n!!! Impossible state !!!");
    return 1;
}

String getUser(String hueRL) {
    // 16 byte hex / 32 nibble hex, just take it as a string
    // the 33'rd char holds 0 for the string terminator
    // char userName[33] = "27787d893751a7726400ccd3c6db19b";
    
    // Get the user from the EEPROM
    EEPROM.get(configAddress, config);
    
    // Check if we have a valid user by probing the first character and testing it on the Hue
    if (config.userName[0] > 0) {
        
        int resp = getLightsConfig(hueRL, String(config.userName));
        
        // Everything else means there is a problem and we need a new user
        if (resp == 0) {
            return config.userName;
        }
    }
    
    Serial.println("No (valid) user, setting up a new one");
    Serial.flush();
    
    // This method blocks until we have a user or an insolvable error
    if (doNewUserRegistration(hueRL, config.userName) > 0) {
        return "";
    }
    
    Serial.print("Received user: ");
    Serial.println(config.userName);
    Serial.flush();
    
    EEPROM.put(configAddress, config);
    
    // Get the lights configuration. If it fails there's nothing to do anymore
    if (getLightsConfig(hueRL, config.userName) != 0) {
        return "";
    }
    
    // All set
    return String(config.userName);
}

String getLightIdFor(String lightName) {
    // TODO: Get from Brigde
    return "1";
}

void enableAlert(String hueRL, String hueser, String lightId, bool once = false) {
    Serial.println("Enabling alert");
    String url = buildAlertUrl(hueRL, hueser, lightId);
    
    doPutRequest(url, once ? "{\"alert\":\"select\"}" : "{\"alert\":\"lselect\"}");
    
    // write the result to serial
    while(p.available()) {
        char c = p.read();
        Serial.print(c);
    }
    p.flush();
    p.close();
}

void disableAlert(String hueRL, String hueser, String lightId) {
    Serial.println("Disabling alert");
    String url = buildAlertUrl(hueRL, hueser, lightId);
    
    doPutRequest(url, "{\"alert\":\"none\"}");
    
    // write the result to serial
    while(p.available()) {
        char c = p.read();
        Serial.print(c);
    }
    p.flush();
    p.close();
}

void setupHue() {
    Serial.println("Setup Hue");
    
    String hueRL = "";
    while (hueRL == "") {
        hueRL = getHueRL();
        if (hueRL == "") {
            fadeForMillis(6000, 10);
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
