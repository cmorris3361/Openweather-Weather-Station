/*
   *****************************************************************************************
   *** Author: Charlie Morris                                                            ***
   *** Version: 0.1                                                                      ***
   *** Last Update: 27/12/24                                                             ***
   *** Usage: Weather API Library to obtain Weather data from https://openweathermap.org ***
   *** 1,000 API calls per day for free                                                  ***
    ****************************************************************************************
*/


#ifndef openweather_h
#define openweather_h

#include <Arduino.h>
#include <WiFiClientSecure.h>   // tzapu/WiFiManager@^2.0.17
#include <HTTPClient.h>

    // Field Types See: https://www.weatherapi.com/docs/
    struct _currentWeatherData {
        uint16_t id;
        String main;
        String description;
        String icon;
        float temp;
        float feels_like;
        float temp_min;
        float temp_max;
        uint16_t pressure;
        uint16_t humidity;
        uint16_t sea_level;
        uint16_t grnd_level;
        uint16_t visibility;
        float wind_speed;
        uint16_t wind_deg;
        float wind_gust;
        float rain;
        uint16_t cloud_cover;
        long dt;
        String country;
        long sunrise;
        long sunset;
        String name;
        uint16_t timezone;
        bool is_day;
        String beaufortScale;
    };

    enum owUnit {standard, metric, imperial };
    
  

 

class openweather
{
private:
    const String baseURL = "https://api.openweathermap.org/data/2.5/weather?";
    const String iconURL = "http://openweathermap.org/img/wn/"; // see https://openweathermap.org/weather-conditions

    // OpenWeather Parameters - see https://openweathermap.org/current#format
    String param_AppID;
    String param_Units = "metric";    // see https://openweathermap.org/current#data
    String param_Lang = "en";           // see https://openweathermap.org/current#data

    bool param_Debug = true;
    int httpCode;
    const char* rootCert= "";
    
    float kevinToCelsius(float kelvin ) { return kelvin - 273.15; }

/*
 * Return the Beaufort wind force scale
 * see: https://www.metoffice.gov.uk/weather/guides/coast-and-sea/beaufort-scale
 * @param Win Speed in M/S
 * @return Beaufort Wind force
 */
String get_BeaufortScale(float wind_speed)
{
    if (wind_speed < 1) { return "Clam"; }
    else if (wind_speed < 2) { return "Light air"; }
    else if (wind_speed < 3) { return "Light breeze"; }
    else if (wind_speed < 5) { return "Gentle breeze"; }
    else if (wind_speed < 8 ) { return "Moderate breeze"; }
    else if (wind_speed < 11) { return "Fresh breeze"; }
    else if (wind_speed < 14) { return "Strong breeze"; }
    else if (wind_speed < 17) { return "Near gale"; }
    else if (wind_speed < 21) { return "Gale"; }
    else if (wind_speed < 24) { return "Strong gale"; }
    else if (wind_speed < 28) { return "Storm"; }
    else if (wind_speed < 32) { return "Violent storm"; }
    else { return "Hurricane"; }
}   

 String makeHTTPCall(String url) 
 {
    String payload;
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client) {
        client->setCACert(rootCert);           // set secure client with certificate 
        client->setInsecure();  // **** This is a Work Around as Root Cert is wrong ***

        HTTPClient https;       // Create an HTTP Client instance

        //Initializing an HTTPS communication using the secure client
        Serial.print("[HTTPS] begin...\n"); 
        if (https.begin(*client, url)) {  // HTTPS
            Serial.print("[HTTPS] GET...\n"); 
            // start connection and send HTTP header
            httpCode = https.GET();       // httpCode will be negative on error

            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payload = https.getString();
                    if (param_Debug) { Serial.printf("Returned Data:\n%s\n", payload.c_str()); }
                } else {
                    Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
            }
            https.end();
        }
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }
    Serial.printf("*** HTTP code: %i ***\n", httpCode);
    return payload;
}


public:


    // *** GETTERS ***
    void get_CurrentWeather(_currentWeatherData *data, float lat, float lon);
    int get_HTTPcode() { return httpCode; }     // Return the HTTP Code

    // *** SETTERS ***
    void set_APIKey(String _key) { param_AppID = _key; }
    void set_Lang(String _lang)  { param_Lang  = _lang; }
    void set_Units(owUnit _units)  { param_Units = _units; }
    void set_Debug(bool _debug)  { param_Debug = _debug; } 



}; // End of Class

#endif