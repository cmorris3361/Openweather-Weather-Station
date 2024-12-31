
#include <Arduino.h>
#include <TimeLib.h>    // Time @ 1.6.1


/*
 * Summary: Convert Meters per Second (MPS) to Mile per Hour (MPH)
 * @Params: Speed in Meters per Second
 * @Return: Speed in Mile per Hour
*/
float MPS2MPH(float MPS){ return MPS * 2.23693629; }

/*
 * Summary: Convert Meters per Second (MPS) to Kilometer per Hour (KPH) 
 * @Params: Speed in Meters per Second
 * @Return: Speed in KiloMeters per Hour
*/
float MPS2KPH(float MPS){ return MPS * 3.6; }

/*
  * Summary: Get te Compass Heading ( 1 of 16 directions)
  * @Params: Heading in Degrees
  * @Return: Heading as 1-3 letters
*/
String compassDirection[16] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
String get_compassDirection(float degrees){
    while (degrees > 360) { degrees = degrees - 360; }  // Ensure heading is 0 to 360
    uint8_t idx = int((degrees + 11.25) / 22.5);
    if (idx > 15) { idx = 0; }     // Allow N to be 348.5 - 0
    return compassDirection[idx];
}

/*
  * Summary: Convert EPOCh time to HH:MM
  * @Params: EPOCH time
  * @Return: Time as HH:MM
*/
String epochHHMM(long epoch) {
  char buff[6];

  double min = minute(epoch);   // This does not work right now
  if (second(epoch) >=30 ) { min++; }   // Round up to nearest minute

  sprintf(buff, "%02d:%02d", hour(epoch), minute(epoch));
  return String(buff);
}


/*
  * Summary: Convert EPOCH time to DD/MM/YY HH:MM:SS
  * @Params: EPOCH time
  * @Return: Date and Time as DD/MM/YY HH:MM:SS
*/
String epochDateTime(long epoch){
  char buff[32];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(epoch), month(epoch), year(epoch), hour(epoch), minute(epoch), second(epoch));
  return String(buff);
}