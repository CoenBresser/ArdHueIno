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
// For some reason passing this as a parameter results in not being able to read the stream...
SerialLogWriter sLog;
FileLogWriter fLog;
TwoLogWriters tLog = TwoLogWriters(fLog, sLog);

/***** Private methods section *****/


void setupHue() {
    Logger.info("Hue init");
    Hue.begin([] () { LedControl::fadeForMillis(10000, 10); });
    
    // We have a valid Hue configuration now
    Logger.info("Notifying we're there");
    
    // Enable alert on the light called "Color Light"
    Hue.revealSelectedLights([] () { LedControl::fadeForMillis(10000, 10); });
}

/***** Public methods section *****/

void setup() {
    Bridge.begin();
    
    Logger.registerLogWriter(tLog);
    Logger.setLogLevel(LOG_LEVEL_DEBUG);
    
    Logger.info("Started");
    
    // Do the hue setup
    setupHue();
}

void loop() {
    Logger.info("Restarting hue cycle");
    
    Logger.info("Wait for 10 seconds");
    LedControl::fadeForMillis(10000, 5);
    
    Logger.info("Set and reset");
    Hue.storeLightStates();
    Hue.setLightStates(true, 254, 0, 254); // Full brightness red
    delay(10000);
    Hue.restoreLightStates();
}
