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

#include "Logger.h"
#include "SerialLogWriter.h"
#include "FileLogWriter.h"
#include "TwoLogWriters.h"
#include "Hue.h"
#include "LedControl.h"
#include <Console.h>

SerialLogWriter sLog;
FileLogWriter fLog;
TwoLogWriters tLog = TwoLogWriters(fLog, sLog);

void setup() {
    Bridge.begin();
    
    Logger.registerLogWriter(tLog);
    Logger.setLogLevel(LOG_LEVEL_DEBUG);
    
    Logger.info("Started");
    
    // Do the hue setup
    Logger.info("Hue init");
    Hue.begin([] () { LedControl::fadeForMillis(10000, 10); });
    
    // We have a valid Hue configuration now
    Logger.info("Notifying we're there");
    
    
    // Enable alert on the light called "Color Light"
    Hue.revealSelectedLights([] () { LedControl::fadeForMillis(5000, 10); });

    Logger.info("Store states initially");
    Hue.storeLightStates();

/*
    // ******** Perform test ******** //

    Logger.info("Set to red");
    Hue.setLightStates(true, 254, 0, 254); // Full brightness red
    
    LedControl::fadeForMillis(10000, 50);
    
    Logger.info("Restore states");
    Hue.restoreLightStates();
 */
}

void loop() {
    Process p;
    
    // This command line runs the WifiStatus script, (/usr/bin/pretty-wifi-info.lua), then
    // sends the result to the grep command to look for a line containing the word
    // "Signal:"  the result is passed to this sketch:
    p.runShellCommand("/usr/bin/pretty-wifi-info.lua | grep Signal | egrep -o '[0-9]{1,3}'");
    
    // do nothing until the process finishes, so you get the whole output:
    while(p.running());
    
    // Read command output. runShellCommand() should have passed "Signal: xx&":
    while (p.available()) {
        int result = p.parseInt();                  // look for an integer
        if (result == 0) continue;
        int signal = map(result, 40, 75, 0, 255);   // map result from 0-100 range to 0-255
        LedControl::setBrightness(signal);          // set the brightness of LED on pin 9
        Serial.println(result);                     // print the number as well
        
        Hue.setLightStates(true, signal, -1, -1);
    }
    delay(1000);  // wait 5 seconds before you do it again
}
