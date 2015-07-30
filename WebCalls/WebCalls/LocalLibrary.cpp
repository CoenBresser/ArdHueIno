//
// LocalLibrary.cpp 
// Library C++ code
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project 		WebCalls
//
// Created by 	Coen Bresser, 30/07/15 13:36
// 				Coen Bresser
//	
// Copyright 	© Coen Bresser, 2015
// Licence 		<#license#>
//
// See 			LocalLibrary.cpp.h and ReadMe.txt for references
//


#include "LocalLibrary.h"

void blink(uint8_t pin, uint8_t times, uint16_t ms) {
  for (uint8_t i=0; i<times; i++) {
    digitalWrite(pin, HIGH); 
    delay(ms >> 1);               
    digitalWrite(pin, LOW);  
    delay(ms >> 1);              
  }
}
