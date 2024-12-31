
// ** NTP Time Server Settings
const char* ntpServer = "pool.ntp.org";     // NTP Server(s)
const char *time_zone = "GMT-0M3.5.0,M10.5.0/2";  // TimeZone rule for London including daylight adjustment rules 

/* for UK
    DLS Starts on: Sunday, 31 March 2024, 01:00:00 (+1hr)
    DLS End on: Sunday, 27 October 2024, 02:00:00 (-1hr)
*/ 

/* 
    So for CET-1CEST,M3.5.0,M10.5.0/3:
    The standard timezone is CET (Central European Time)
    The offset from UTC is −1
    The DST timezone is CEST (Central European Summer Time)
    DST starts at:
        3: the third month of the year (March)
        5: the last…
        0: …Sunday of the month
    (no time specifier, defaults to 2 AM)
    DST ends at:
        10: the tenth month of the year (October)
        5: the last…
        0: …Sunday of the month
        3: at 3 AM
*/

// OpenWeather Parameters
const String APIKey = "YOUR OPENWEATHER API KEY HERE";   // Your OpenWeather API Key
const float Latitude = 52.8073; // Your Location in Longitude and Latitude
const float Longitude = -1.6426;

//const float Latitude = 12.562091;
//const float Longitude = 99.957687;

#define weatherPoll 15  // Time in Minutes between getting the Weather

