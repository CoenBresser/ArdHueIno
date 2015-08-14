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
//#include "FileLogWriter.h"
//#include "TwoLogWriters.h"
#include "Hue.h"
#include "LedControl.h"
#include <Console.h>

SerialLogWriter sLog;
//FileLogWriter fLog;
//TwoLogWriters tLog = TwoLogWriters(fLog, sLog);

void setup() {
    Bridge.begin();
    
    Logger.registerLogWriter(sLog);
    Logger.setLogLevel(LOG_LEVEL_DEBUG);
    
    Logger.info("Started");
    
    // Do the hue setup
    Logger.info("Hue init");
    Hue.begin([] () { LedControl::fadeForMillis(10000, 10); });
    
    // We have a valid Hue configuration now
    Logger.info("Notifying we're there");
    
    
    // Enable alert on the light called "Color Light"
    Hue.revealSelectedLights([] () { LedControl::fadeForMillis(5000, 10); });

    /*
    // ******** Perform test ******** //
    Logger.info("Store states");
    Hue.storeLightStates();
    
    Logger.info("Set to red");
    Hue.setLightStates(true, 254, 0, 254); // Full brightness red
    
    delay(10000);
    Logger.info("Restore states");
    Hue.restoreLightStates();
    */
}

void loop() {
    delay(100);
}
