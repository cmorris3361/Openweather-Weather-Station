
#include <Arduino.h>
#include "openweather.h"
#include <ArduinoJson.h>


/*
 * Get the Current Weather from weatherAPI
 * @param data Structure & Latitude and Longitude
 * @return none
 */

void openweather::get_CurrentWeather(_currentWeatherData *data, float lat, float lon){
  long startTime = millis();

  // Construct the calling URL ()
  String url = baseURL + "lat=" + String(lat) +
                         "&lon=" + String(lon) +
                         "&units=" + param_Units +
                         "&lang=" + param_Lang +
                         "&appid=" + param_AppID;
  Serial.printf("Obtaining current Weather data from WeatherAPI.io API, with calling URL: \n%s\n\n", url.c_str()); 
  String jsonWeatherdata = makeHTTPCall(url.c_str());   // Make the HTTP call to obtain the Curent Weather

  // Now we need to extract the Weather data from the String to add to the Data Structures
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, jsonWeatherdata);

  if (error) {
    // Unable to Deserialise Weather data
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    httpCode = 0;
  } else {
    // JSON Weather data to is OK to Deserialise
    data->id                = doc["weather"][0]["id"].as<uint16_t>();
    data->main              = doc["weather"][0]["main"].as<String>();
    data->description       = doc["weather"][0]["description"].as<String>();
    data->icon              = doc["weather"][0]["icon"].as<String>();

    data->temp              = doc["main"]["temp"].as<float>();
    data->feels_like        = doc["main"]["feels_like"].as<float>();
    data->temp_min          = doc["main"]["temp_min"].as<float>();
    data->temp_max          = doc["main"]["temp_max"].as<float>();
    data->pressure          = doc["main"]["pressure"].as<uint16_t>();
    data->humidity          = doc["main"]["humidity"].as<uint16_t>();
    data->sea_level         = doc["main"]["sea_level"].as<uint16_t>();
    data->grnd_level        = doc["main"]["grnd_level"].as<uint16_t>();

    data->visibility        = doc["visibility"].as<uint16_t>();

    data->wind_speed        = doc["wind"]["speed"].as<float>();
    data->wind_deg          = doc["wind"]["deg"].as<uint16_t>();;
    data->wind_gust         = doc["wind"]["gust"].as<float>();;

    data->rain              = doc["rain"]["1h"].as<float>();
    data->cloud_cover       = doc["clouds"]["all"].as<uint16_t>();

    data->dt                = doc["dt"].as<long>();

    data->country           = doc["sys"]["country"].as<String>();
    data->sunrise           = doc["sys"]["sunrise"].as<float>();;
    data->sunset            = doc["sys"]["sunset"].as<float>();;

    data->name              = doc["name"].as<String>();
    data->timezone          = doc["timezone"].as<uint16_t>();


    data->beaufortScale     = get_BeaufortScale(data->wind_speed);
    data->icon = iconURL + data->icon + "@2x.png"; // Add full path to Image - use '@2x.png' for larger image

    if (data->dt >= data->sunrise && data->dt <= data->sunset) { data->is_day = true; } else ( data->is_day = false);
  }
  
  // Rap up
  long processingTime = (millis() - startTime)/1000;
  Serial.print("Pressing time in obtaining the Weather data was ");
  Serial.print(processingTime);
  Serial.println(" seconds.");
}
