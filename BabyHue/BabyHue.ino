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

// For some reason passing this as a parameter results in not being able to read the stream...
Logger logger;
Hue hue;

/***** Private methods section *****/


void setupHue() {
    logger.info("Hue init");
    hue.begin();
    
    // We have a valid Hue configuration now
    logger.info("Notifying we're there");
    
    // Enable alert on the light called "Color Light"
    hue.enableAlert("Color Light");
    LedControl::fadeForMillis(10000, 1);
    hue.disableAlert("Color Light");
}

/***** Public methods section *****/

void setup() {
    Bridge.begin();
    
    SerialLogWriter sLog;
    FileLogWriter fLog;
    TwoLogWriters tLogs = TwoLogWriters(fLog, sLog);
    logger.registerLogWriter(tLogs);
    
    logger.info("Started");
    
    // Do the hue setup
    setupHue();
}

void loop() {
    LedControl::fadeForMillis(5000, 5);
}
