//
// File			LocalLibrary.h
// Brief		Library header
//
// Project	 	WebCalls
// Developed with [embedXcode](http://embedXcode.weebly.com)
// 
// Author		Coen Bresser
// 				Coen Bresser
// Date			30/07/15 13:36
// Version		<#version#>
// 
// Copyright	© Coen Bresser, 2015
// Licence		<#license#>
//
// See			ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#include "LRF.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#include "application.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE


#ifndef WebCalls_LocalLibrary_h
#define WebCalls_LocalLibrary_h

//
// Brief	Blink a LED
// Details	LED attached to pin is light on then light off
// Total cycle duration = ms
// Parameters:
//      pin pin to which the LED is attached
//      times number of times
//      ms cycle duration in ms
//
void blink(uint8_t pin, uint8_t times, uint16_t ms);

#endif
