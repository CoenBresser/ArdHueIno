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

class Hue_ {
public:
    Hue_() { };
    
    void begin(void (*waitFunction)(void) = NULL);
    
    void storeLightStates(void (*waitFunction)(void) = NULL);
    void restoreLightStates(void (*waitFunction)(void) = NULL);
    
    void setLightStates(bool on, int brightness, int hue, int saturation, void (*waitFunction)(void) = NULL);
    void revealSelectedLights(void (*waitFunction)(void) = NULL);    
    
};

extern Hue_ Hue;
#endif /* defined(__BabyHue__Hue__) */
