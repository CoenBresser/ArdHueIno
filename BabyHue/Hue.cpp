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
#include "Logger.h"
#include <EEPROM.h>

/************************************** Setup ****************************************/

void Hue_::begin(void (*waitFunction)(void)) {
    
    Logger.debug("Setting up Hue on Yun");
    
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
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG); // Do something with the received OK
    _hueP.flush();
    _hueP.close();
}

/************************************** Store/restore ********************************/

void Hue_::storeLightStates(void (*waitFunction)(void)) {
    
    Logger.debug("Storing light states");
    
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

void Hue_::restoreLightStates(void (*waitFunction)(void)) {
    
    Logger.debug("Restoring light states");
    
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

/************************************** Manipulation *********************************/

void Hue_::setLightStates(bool on, int brightness, long hue, int saturation, void (*waitFunction)(void)) {
    
    char msg[70];
    sprintf(msg, "Setting lights in group to on:%s, bri:%d, hue:%d, sat:%d", on ? "true":"false", brightness, hue, saturation);
    Logger.debug(msg);
    
    Process _hueP;
    _hueP.begin("/usr/BabyHue/./doSetGroupLightsState.sh");
    _hueP.addParameter(on ? "true":"false");
    _hueP.addParameter(String(brightness));
    _hueP.addParameter(String(hue));
    _hueP.addParameter(String(saturation));
    _hueP.runAsynchronously();
    while (_hueP.running()) {
        delay(100);
    }
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG);
    
    _hueP.flush();
    _hueP.close();
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
    //Logger.dumpStream(_hueP, LOG_LEVEL_DEBUG);
    
    _hueP.flush();
    _hueP.close();
}

Hue_ Hue;
