//
//  LedControl.cpp
//  BabyHue
//
//  Created by Coen Bresser on 31/07/15.
//  Copyright (c) 2015 Coen Bresser. All rights reserved.
//

#include <Bridge.h>
#include "LedControl.h"

void LedControl::fadeForMillis(int totalTime, int stepSize, int led, int delayTime){
    int brightness = 0;
    int fadeAmount = stepSize;
    
    // Just set the pinmode, assume the toolkit / processor handles the error
    pinMode(led, OUTPUT);
    
    for (int i = 0; i <= totalTime / delayTime; i++) {
        // set the brightness of the LED pin:
        analogWrite(led, brightness);
        
        // change the brightness for next time through the loop:
        brightness = brightness + fadeAmount;
        
        // reverse the direction of the fading at the ends of the fade:
        if (brightness == 0 || brightness == 255) {
            fadeAmount = -fadeAmount;
        }
        delay(delayTime);
    }
    analogWrite(led, 0);
}