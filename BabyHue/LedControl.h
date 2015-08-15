//
//  LedControl.h
//  BabyHue
//
//  Created by Coen Bresser on 31/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#ifndef __BabyHue__LedControl__
#define __BabyHue__LedControl__

#include <stdio.h>

class LedControl {
public:
    LedControl() { }
    
    //
    // A blocking method to fade an led
    // The method uses fixed delay times and a variable step, else timing can be off
    //
    // default LED is 13
    // default Delay is 20ms if slower the steps in the fade effect will be visible
    //
    static void fadeForMillis(int totalTime, int stepSize, int led = 13, int delayTime = 20);
    
    // Set a PWM LED to a specific brightness
    static void setBrightness(int brigthness, int led = 13);
    
};
#endif /* defined(__BabyHue__LedControl__) */
