  /* ***********************************
    *** CYD LDR CLASS           ***
    *** Author: Charlie Morris      ***
    *** https://debugdiaries.co.uk/ ***
    *** Version: 0.2                ***
    *** Last update: 30/12/24       ***
    ***************************************************************************************************
    *** Permission is hereby granted, free of charge, to any person obtaining a copy of this        ***
    *** software and associated documentation files. The above copyright notice and this permission ***
    *** notice shall be included in all copies or substantial portions of the Software.             ***
    ***************************************************************************************************
  */

  /* ***************************************************************************************************
     *** Version Control 
     | 0.1 | 17/12/24 | First draft version
     | 0.1 | 30/12/24 | Added "is_Light" boolean First draft version
     ***************************************************************************************************

*/
  
#ifndef CYDLDR_H
#define CYDLDR_H

#include <Arduino.h>

#define LDR_PIN 34  // GPIO Pin of the LDR

class CYD_LDR
{
  public:
  void begin() {
    pinMode(LDR_PIN, INPUT);
    set_isDarkTreshold(400);    // Set the inital is Dark Threshold
  }

  // *** SETTERS ***
  void set_isDarkTreshold(uint16_t value){
    isDarkThreshold = value;
  };

  // *** Getters ***
  
  // Retruns boolean True if Dark , False is Light
  bool get_isDark(){ return isDark; }
  // Retruns boolean True if Dark , False is Light
  bool get_isLight(){ return isLight; }

  // Retruns the value of the is Dark Threshold
  uint16_t get_isDarkThreshold() { return isDarkThreshold; }

  // Retruns the value of the LDR
  uint16_t get_LDR(){
    uint16_t ldr = analogRead(LDR_PIN);
    if (ldr > isDarkThreshold) { 
      isDark = true;
      isLight = false; 
    } else { 
      isDark = false; 
      isLight = true; 
    }
    //Serial.printf("Debug: V: %i, T: %i, Dark: %i\n", ldr, isDarkThreshold, isDark);
    return ldr;
  }

  private:
  bool isDark;
  bool isLight;
  uint16_t isDarkThreshold;

};


#endif // End of LSR Class
