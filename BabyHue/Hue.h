//
//  Hue.h
//  BabyHue
//
//  Created by Coen Bresser on 04/08/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__Hue__
#define __BabyHue__Hue__

#include "Logger.h"

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
    
    String hueRL;
    String hueser;
    
    String getHueRL();
    String doGetValidateHueser(void (*waitFunction)(void) = NULL);
    
    String buildLightsBaseUrl();
    String buildLightStatePutUrl(int id);
    String buildLightGetUrl(int id);
    
    String doGetLightsConfig();
    String doNewUserRegistration(void (*waitFunction)(void) = NULL);
    
    void checkCreateHueGroup();

    void setLightState(int lightId, String& on, String& brightness, String& hue, String& saturation);
    void setLightStates(String& on, String& brightness, String& hue, String& saturation);
};

extern Hue_ Hue;
#endif /* defined(__BabyHue__Hue__) */
