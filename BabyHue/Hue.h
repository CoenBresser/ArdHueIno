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

class Hue {
public:
    Hue(int eepromBaseAddress = 0) : eepromBaseAddress(eepromBaseAddress) { };
    
    void begin();
    
    void enableAlert(String lightName, bool once = false);
    void enableAlert(int lightId, bool once = false);
    
    void disableAlert(String lightName);
    void disableAlert(int lightId);
    
private:
    int eepromBaseAddress;
    
    String hueRL;
    String hueser;
    
    String getHueRL();
    String doGetValidateHueser();
    
    String buildLightsBaseUrl();
    String buildAlertUrl(int id);
    
    String doGetLightsConfig();
    String doNewUserRegistration();

};

#endif /* defined(__BabyHue__Hue__) */
