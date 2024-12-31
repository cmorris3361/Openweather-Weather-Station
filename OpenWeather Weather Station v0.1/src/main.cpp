/*
  *****************************************************************************************
  *** Author: Charlie Morris                                                            ***
  *** Version: 0.1                                                                      ***
  *** Last Update: 28/12/24                                                             ***
  *** Usage: Weather Station powered by https://openweathermap.org                      ***
  *****************************************************************************************
*/

/* Known issues
  +----+-----------------------------------------------------------------------------------
  | ## | Description
  +----+-----------------------------------------------------------------------------------
  | 01 | When pngle only works with HTTP call not HTTPS
  +----+-----------------------------------------------------------------------------------
*/



#include <Arduino.h>

#include <TFT_eSPI.h>     // bodmer/TFT_eSPI@^2.5.43
#include <WiFiManager.h>  // tzapu/WiFiManager@^2.0.17 - https://github.com/tzapu/WiFiManager
#include <pngle.h>        // kikuchan98/pngle@^1.0.2


#include "openweather.h"

#include <version.h>

#include "settings.h"
#include "usersettings.h"
#include "functions.h"
#include "CYDLDR.h"




TFT_eSPI tft = TFT_eSPI();  // Create the TFT Instance
TFT_eSprite spr_Weather = TFT_eSprite(&tft);  // Create TFT instance for the Weather data
TFT_eSprite spr_Compass = TFT_eSprite(&tft);
TFT_eSprite spr_CompassNeedle = TFT_eSprite(&tft);

CYD_LDR LDR;


_currentWeatherData currentWeather;


struct tm tmstruct;
time_t epochtime;

uint64_t msNow;
uint64_t msSecond;
uint16_t weatherNow = 6400;

// *** Function Declarations ***
void initCYD();
void writeStatusbar(String text, uint16_t Y);
void connectWifi();
void showWiFiRRSI();
void setTime();
void displayClock();
void getWeather();
void displayWeather();
void displayImage(String url, uint16_t x, uint16_t y, double scale);
void set_CYDBackLight(uint8_t percent);
void createCompass();
void createCompassNeedle();


// *** Inital Setup (runonce) ***
void setup() {
  Serial.begin(115200);   	// Initialise Serial Port
  Serial.printf("Weather Station Project - Version: %s\n", VERSION);


  initCYD(); 	// Initialise CYD Display


  tft.fillRect(0, SBY, CYD_WIDTH, SBH, TFT_WHITE);   // Draw Status Bar
  writeStatusbar(VERSION_SHORT, 5);
  writeStatusbar("powered by openweather", 170);

  connectWifi(); // Connect to WiFi
 
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("Obtaining the current time.", CYD_WIDTH_CENTER, 110, mFONT_SIZE);
  setTime();
  
  tft.fillRect(0, 0, CYD_WIDTH, SBY - 1, bkColor);     // Clear the main screen (not Status bar)
  tft.drawFastHLine(0, weatherY-1, CYD_WIDTH, TFT_DARKGREY);  // Draw Horizontal line top separate Date & time and weather 

  createCompass();
  createCompassNeedle();
}


void loop() {
  msNow = millis();
  // One Second Poll
  if (msNow >= msSecond) {
    msSecond = millis() + 1000;
    weatherNow++;
    showWiFiRRSI(); // Display an indicator of the level of the WiWi RSSI value

    displayClock(); // Display the Clock and Date 

    // Change Brightness of Screen using the LDR
    int ldrValue = LDR.get_LDR();
    if (ldrValue < 100) {
        set_CYDBackLight(100);    // Set Backlight 100%
    } else {
        set_CYDBackLight(5);    // Set Backlight 10%
    }

    // Check to see if we need to get the weather 
    if (weatherNow >= weatherPoll * 60){
      weatherNow = 0;
      getWeather();

      // Show Day time ot Night icon on Status bar
      uint16_t c;
      if (currentWeather.is_day) { c = TFT_ORANGE; } else { c = TFT_BLACK; }
      tft.fillCircle(70, SBY + 5, 4, c); // Show Day or Night on Status Bar
               
      displayWeather();
    }
  }
}

// *******************************************************
// *** Functions  ***
// *******************************************************
/*
* @Summary Sets the Backlight between 0 and 100
* @param Percentage Value (0-100)
* @return none
*/
void set_CYDBackLight(uint8_t percent){
  // Cap the the percentage value to 100
  if (percent > 100 ) {percent = 100; }   
  // Change the percentage value to 0 - 255 (Rounded to the nearest Integer)
  percent = int((percent * 2.55) + .5);
  // Write the value to the Backlight GPIO pin
  analogWrite(BL_GPIO, percent);
}

/*
 * Initialise and Clear the CYD Display
 * @param none
 * @return none
 */
void initCYD(){
  tft.init();  // Initialise TFT display
  tft.setRotation(LANDSCAPE_180); 
  CYD_WIDTH  = tft.width();
  CYD_HEIGHT = tft.height();
  CYD_WIDTH_CENTER  = CYD_WIDTH / 2;
  CYD_HEIGHT_CENTER = CYD_HEIGHT / 2;
  
  pinMode(BL_GPIO, INPUT); // Set Backlight GPIO pin as an Input
  set_CYDBackLight(100);   // Set Backlight to 100% (full brightness)

  tft.fillScreen(bkColor);  // Clear Display
  tft.drawCentreString("Weather Station (" + String(VERSION_SHORT) + ")", CYD_WIDTH_CENTER, 4, lFONT_SIZE);
  tft.drawCentreString("Powered by OpenWeather", CYD_WIDTH_CENTER, 30, mFONT_SIZE);
  tft.drawCentreString("Please wait while I get things ready..", CYD_WIDTH_CENTER, 50, mFONT_SIZE);


}

/*
* Write to Statusbar at X position
* @param Text to Display, X Axis Position
* @return none
*/
void writeStatusbar(String text, uint16_t X){
  tft.setTextColor(SBFC, SBBC);
  tft.drawString(text, X, SBY + 1, sFONT_SIZE);
}

void connectWifi(){
  tft.setTextColor(TFT_WHITE, bkColor);
  tft.drawCentreString("Connecting to WiFi", CYD_WIDTH_CENTER, 80, mFONT_SIZE);

  WiFiManager wm;

  wm.setTimeout(10); //Set Timeout trying to connect (in seconds)
  
  // Uncomment to Reset WiFi settings - wipe stored credentials for testing these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  bool res = wm.autoConnect("Weather_Station_AP","password"); // password protected ap

   
  if(!res) {
    Serial.println("Failed to connect");
    tft.setTextColor(TFT_RED, bkColor);
    tft.drawCentreString("Failed to connect to WiFi", CYD_WIDTH_CENTER, 96, mFONT_SIZE);
    // ESP.restart();
  } else {
    // Successfully connected to WiFi
    Serial.printf("Successfully connected to %s (IP: %s)\n",WiFi.SSID(), WiFi.localIP().toString());

    tft.setTextColor(TFT_GREEN, bkColor);
    tft.drawCentreString("Successfully to connect to WiFi (" + WiFi.localIP().toString() + ")", CYD_WIDTH_CENTER, 96, mFONT_SIZE);
  }
}
/*
* Get the Wifi RSSI signal Strength and show status of it on the Status Bar
* RSSI < -90 dBm: Extremely weak signal.
* RSSI -67 dBm: Fairly strong signal.
* RSSI > -55 dBm: Very strong signal.
* RSSI > -30 dBm: Signal is right next to the transmitter.
* @param none
* @return none
*/
void showWiFiRRSI(){
  int rssi = WiFi.RSSI  ();
  uint16_t rssiColor;
  if (rssi > -55) { rssiColor = TFT_GREEN; }
  else if (rssi > -67) { rssiColor = TFT_YELLOW; }
  else { rssiColor =TFT_RED; }

  tft.fillCircle(310, SBY + 5, 4, rssiColor); // Show RSSI level on Status Bar

}

/*
* Set the time
* @param none
* @return none
*/
void setTime(){
    // Get The time
    Serial.println("Contacting Time Server");
    configTzTime(time_zone, ntpServer);

    delay(2000);
    getLocalTime(&tmstruct, 5000);

    char timeBuffer[30];
    sprintf(timeBuffer,"%d-%02d-%02d, %02d:%02d:%02d",(tmstruct.tm_year)+1900,(tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
    Serial.printf("Local Time: %s (time Zone: %s)\n",String(timeBuffer).c_str(), String(time_zone).c_str());

    time(&epochtime);
    Serial.print("Epoch time (UTC): ");
    Serial.println(epochtime);
    Serial.println();
}

/*
* Set the time
* @param none
* @return none
*/
void displayClock(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");

    ESP.restart();

    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // Date (
  char bufferDate[30];
  sprintf(bufferDate, "%s %02d %s", shortDay[timeinfo.tm_wday], timeinfo.tm_mday, shortMonth[tmstruct.tm_mon]);
  tft.setTextColor(dateColor, bkColor);
  tft.drawCentreString(String(bufferDate), CYD_WIDTH_CENTER, 0, lFONT_SIZE);

  // Time (HH:MM)
  char bufferHHMM[6];
  sprintf(bufferHHMM, "%02d:%02d",timeinfo.tm_hour, timeinfo.tm_min);
  tft.setTextColor(clockColor, bkColor);
  tft.drawCentreString(String(bufferHHMM), CYD_WIDTH_CENTER, 30, xxlFONT_SIZE);
  
}

/*
* Get the weather
* @param none
* @return none
*/
void getWeather(){
  Serial.println("Getting the Weather");

  openweather *currentWeatherClient = new openweather();
   
  // Setup Weather Parameters
  currentWeatherClient->set_APIKey(APIKey);
  currentWeatherClient->set_Debug(true);
  //currentWeatherClient->set_Lang("de"); // Optional
  //currentWeatherClient->set_Units(owUnit.); // Optional
   
  currentWeatherClient->get_CurrentWeather(&currentWeather, Latitude, Longitude);
  

  delete currentWeatherClient;
  currentWeatherClient = nullptr;

  Serial.printf("\nWeather in %s is %s\n", currentWeather.name.c_str(), currentWeather.description.c_str());
  Serial.printf("Temperature is %.1fC, but feels like %.1fC\n", currentWeather.temp, currentWeather.feels_like);
  Serial.printf("With a Minimum Temperature of %.1fC and Maximum Temperature of %.1fC\n", currentWeather.temp_min, currentWeather.temp_max);
  Serial.printf("Pressure is %ihpa and Humidity is %i%% \n", currentWeather.pressure, currentWeather.humidity);
  Serial.printf("Cloud Cover is %i%%\n", currentWeather.cloud_cover);
  Serial.printf("Rain over the next hour %.1fmm/hr\n", currentWeather.rain);
  Serial.printf("Wind is blowing %.1fm/s (%.1fmph) with gusts of %.1fm/s (%.1fmph) with a heading of %s (%i deg).\n", 
                    currentWeather.wind_speed, MPS2MPH(currentWeather.wind_speed),
                    currentWeather.wind_gust,  MPS2MPH(currentWeather.wind_gust),
                    get_compassDirection(currentWeather.wind_deg), currentWeather.wind_deg);
  Serial.printf("The wind is %s\n", currentWeather.beaufortScale.c_str());
  Serial.print("Sunrise "); Serial.println(currentWeather.sunrise);
  Serial.print(" Sunset "); Serial.println(currentWeather.sunset);
  if (currentWeather.is_day) { Serial.println("Is Day Time."); } else { Serial.println("Is Night Time"); }
  Serial.printf("Time Zone: %i\n", currentWeather.timezone);
  Serial.println(currentWeather.dt);
  Serial.println(epochHHMM(currentWeather.dt));
  Serial.println(epochDateTime(currentWeather.dt));
  Serial.println();

}

/*
* Display the Weather 
* @param none
* @return none
*/
void displayWeather(){


  spr_Weather.createSprite(CYD_WIDTH, weatherTileH);
  spr_Weather.fillSprite(bkColor);
  spr_Weather.setColorDepth(8);

  // Weather conditions
  String verb = " with ";
  if (currentWeather.beaufortScale == "Calm") { verb = " and "; }
  
  String conditions1 = currentWeather.description + verb + currentWeather.beaufortScale;
  String conditions2; 
  uint8_t maxLineLength = 30;
  // Split the Line if it too long
  if (conditions1.length() > maxLineLength){
    // Find first occurrence of " " back from max line Length to split long line to 2 lines
    // Convert String to a Chararray`
    char ca[conditions1.length() + 1];
    conditions1.toCharArray(ca, conditions1.length());
    // Find space
    uint8_t idx;
    for (idx = maxLineLength; idx > 0; idx--){ if (ca[idx] == ' ') {  break; } }
    conditions2 = conditions1.substring(idx + 1, conditions1.length());
    conditions1 = conditions1.substring(0, idx);
  }
  
  spr_Weather.setTextColor(TFT_WHITE);
  spr_Weather.drawString(conditions1, 0, 73, mFONT_SIZE);
  spr_Weather.drawString(conditions2, 0, 88, mFONT_SIZE);  

  // Display Current Temperature
  spr_Weather.setCursor(100, 10);
  spr_Weather.setTextFont(xlFONT_SIZE); 
  spr_Weather.printf("%0.1f", currentWeather.temp);
  spr_Weather.setTextFont(lFONT_SIZE);
  spr_Weather.print("oC");

  // Display High & Low temperatures
  spr_Weather.setCursor(100, 47);
  spr_Weather.setTextColor(TFT_YELLOW);
  spr_Weather.setTextFont(mFONT_SIZE);
  spr_Weather.printf("L:%.0f H:%.0f FL:%.1f", currentWeather.temp_min, currentWeather.temp_max, currentWeather.feels_like);
  
  // // Show Compass Needle
  spr_CompassNeedle.pushRotated(&spr_Compass, currentWeather.wind_deg + 180); // Draw Compass Needle
  
  // Display Compass
  spr_Compass.pushToSprite(&spr_Weather, compassX - compassR, compassY - compassR); //Push sprite at Top, Left not Centre

  uint8_t x = compassX - compassR + 20;
  uint8_t y = (compassY * 2) + 2;
  spr_Weather.setTextColor(TFT_YELLOW);
  spr_Weather.drawCentreString(String(currentWeather.wind_deg), compassX, compassY - 15, sFONT_SIZE);
  spr_Weather.drawCentreString(get_compassDirection(currentWeather.wind_deg), compassX, compassY + 10, sFONT_SIZE);
  spr_Weather.setTextFont(sFONT_SIZE);
  spr_Weather.setCursor(x, y);
  spr_Weather.printf("S:%.0fmph", MPS2MPH(currentWeather.wind_speed));
  spr_Weather.setCursor(x, y + 10);
  spr_Weather.printf("G:%.0fmph", MPS2MPH(currentWeather.wind_gust));
  
  // Push Weather Tile Sprite to Display
  spr_Weather.pushSprite(0, weatherY);
  spr_Weather.deleteSprite();


  displayImage(currentWeather.icon.c_str(), 0, weatherY, 1);   // Display the current Weather Icon, need to do it at the end as it write over the Sprite

  Serial.println("Done with displaying Weather data.");

  }


// ===================================================
// pngle example for TFT_eSPI
// ===================================================

double g_scale = 1.0;
uint16_t png_X = 0; 
uint16_t png_Y = 0;

// pngle callback, called during decode process for each pixel
void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  x = x + png_X;
  y = y + png_Y;

  // Convert to RGB 565 format
  uint16_t color = tft.color565(rgba[0], rgba[1], rgba[2]);

  // If alpha > 0 then draw
  if (rgba[3]) {
    if (g_scale <= 1.0) tft.drawPixel(x, y, color);
    else {
      x = ceil(x * g_scale);
      y = ceil(y * g_scale);
      w = ceil(w * g_scale);
      h = ceil(h * g_scale);
      tft.fillRect(x, y, w, h, color);
    }
  }

}


// <summary>
// Display an Image (PNG) from a given URL
// </summary>
// <param name="URL">URL pf the Image</param>
// <returns>None</return
void displayImage(String url, uint16_t x, uint16_t y, double scale){
  Serial.printf("Image URL: %s\n", url.c_str());
  png_X = x;
  png_Y = y;

  HTTPClient http;

  http.begin(url);

  int httpCode = http.GET();

  Serial.printf("HTTP Code: %i\n", httpCode);

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP ERROR: %u\n", httpCode);
    http.end();
    return ;
  }

  int total = http.getSize();
  Serial.printf("Image Size: %ibytes.\n", total);


  WiFiClient *stream = http.getStreamPtr();

  pngle_t *pngle = pngle_new();
  pngle_set_draw_callback(pngle, pngle_on_draw);
  g_scale = scale;

  uint8_t buf[2048];
  int remain = 0;
  uint32_t timeout = millis();
  while (http.connected()  && (total > 0 || remain > 0)) {

    // Break out of loop after 10s
    if ((millis() - timeout) > 10000UL)
    {
      Serial.printf("HTTP request timeout\n");
      break;
    }

    size_t size = stream->available();

    if (!size) { delay(1); continue; }

    if (size > sizeof(buf) - remain) {
      size = sizeof(buf) - remain;
    }

    int len = stream->readBytes(buf + remain, size);
    if (len > 0) {
      int fed = pngle_feed(pngle, buf, remain + len);
      if (fed < 0) {
        Serial.printf("ERROR: %s\n", pngle_error(pngle));
        break;
      }
      total -= len;
      remain = remain + len - fed;
      if (remain > 0) memmove(buf, buf + fed, remain);
    } else {
      delay(1);
    }
  }

  pngle_destroy(pngle);

  http.end();
}



/*
* "Summary Create Sprite for the Compass 
* @param none
* @return none
*/
void createCompass(){
  spr_Compass.createSprite(compassR * 2, compassR * 2);
  spr_Compass.setColorDepth(16);

  // Draw Outer body of Compass
  spr_Compass.drawCircle(compassR, compassR, compassR-1, TFT_LIGHTGREY);
  spr_Compass.fillCircle(compassR, compassR, compassR / 2, TFT_DARKGREY);
  
  // Draw Markers - Hijack the Needle Sprite
  spr_CompassNeedle.createSprite(5, compassR);
  spr_CompassNeedle.setColorDepth(16);
  spr_CompassNeedle.setPivot(3, compassR);
  spr_CompassNeedle.fillCircle(3, 2, 2, TFT_WHITE); // Draw Marker
  for (uint8_t x; x < 8; x++){
    spr_CompassNeedle.pushRotated(&spr_Compass, x * (366 / 8), TFT_BLACK);
  }
  // Draw Compass Headings
  spr_Compass.setTextColor(TFT_RED);
  spr_Compass.drawCentreString("N", compassR, 2, mFONT_SIZE);
  spr_Compass.setTextColor(TFT_WHITE);
  spr_Compass.drawCentreString("S", compassR, (compassR * 2)- 12, sFONT_SIZE);
  spr_Compass.drawCentreString("W", 8, compassR - 7, sFONT_SIZE);
  spr_Compass.drawCentreString("E", compassR + compassR - 7, compassR - 4, sFONT_SIZE);

  spr_CompassNeedle.deleteSprite();

}
  
/*
* "Summary Create Sprite for the Compass Needle
* @param none
* @return none
*/
void createCompassNeedle(){
  uint8_t needleLength =  (compassR * 2) - 10;
  spr_CompassNeedle.createSprite(7, needleLength);
  uint8_t x = spr_CompassNeedle.width() / 2;
  uint8_t y = spr_CompassNeedle.height() / 2;
  spr_CompassNeedle.setColorDepth(8);
  spr_CompassNeedle.fillSprite(TFT_TRANSPARENT);
  spr_CompassNeedle.setPivot(x, y);   // Set Sprite pivot point to centre of Sprite
  
  spr_CompassNeedle.fillTriangle(x, 0, 0, 10, 7, 10, TFT_RED);  // Needle Arrow Head
  spr_CompassNeedle.drawFastVLine(x, 10,  needleLength - 10, TFT_YELLOW);       // Needle Body
  spr_CompassNeedle.fillCircle(x, needleLength, 2, TFT_WHITE);            // Needle End Dot
  spr_CompassNeedle.fillCircle(x, y, 2, TFT_LIGHTGREY);         // Centre
  spr_CompassNeedle.drawPixel(x, y, TFT_RED);                   // Centre Dot


}