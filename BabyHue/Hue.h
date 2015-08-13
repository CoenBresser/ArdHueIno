//
//  Hue.h
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//
// Warning! Due to the amount of memory used this class uses some globals
//  making it not safe for concurrent access

#ifndef __BabyHue__Hue__
#define __BabyHue__Hue__

#include "Logger.h"

#define MAX_HUE_GROUP_MEMBERS 8

class Hue_ {
public:
    Hue_(int eepromBaseAddress = 0) : eepromBaseAddress(eepromBaseAddress) { };
    
    void begin(void (*waitFunction)(void) = NULL);
    
    void enableAlert(int lightId, bool once = false);
    void disableAlert(int lightId);
    
    void storeLightState(int lightId);
    void restoreLightState(int lightId);
    void storeLightStates();
    void restoreLightStates();
    
    void setLightState(int lightId, bool on, int brightness, int hue, int saturation);
    void setLightStates(bool on, int brightness, int hue, int saturation);
    
    void revealSelectedLights(void (*waitFunction)(void) = NULL);
    
private:
    int eepromBaseAddress;
    
    char hueRL[27]; // e.g. http://192.168.255.255/api/
    char hueser[33]; // e.g. 27787d893751a7726400ccd3c6db19b
    int lightIds[MAX_HUE_GROUP_MEMBERS];
    
    void getHueRL();
    void doGetValidateHueser(void (*waitFunction)(void) = NULL);
    
    const char* buildLightsBaseUrl();
    const char* buildLightStatePutUrl(int id);
    const char* buildLightGetUrl(int id);
    const char* buildGroupsBaseUrl();
    const char* buildGroupActionUrl();
    const char* buildGroupUrl();
    
    String doGetLightsConfig();
    String doNewUserRegistration(void (*waitFunction)(void) = NULL);
    
    void checkCreateHueGroup(void (*waitFunction)(void) = NULL);
    void getLightIdsFromGroup();
};

extern Hue_ Hue;
#endif /* defined(__BabyHue__Hue__) */
