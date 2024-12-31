#include <Arduino.h>
#include <TFT_eSPI.h> 





// *** CYD Display Settings *** 
#define sFONT_SIZE 1
#define mFONT_SIZE 2
#define lFONT_SIZE 4
#define xlFONT_SIZE 6
#define xxlFONT_SIZE 8

#define BL_GPIO 21

const uint8_t PORTRAIT      = 0;
const uint8_t LANDSCAPE     = 1;
const uint8_t PORTRAIT_180  = 2;
const uint8_t LANDSCAPE_180 = 3;

uint16_t CYD_WIDTH = 240;
uint16_t CYD_HEIGHT = 360;
uint16_t CYD_WIDTH_CENTER = 120;
uint16_t CYD_HEIGHT_CENTER = 160;

// *** StatusBar Settings ***
const uint16_t SBBC = TFT_WHITE;              // Background Color of Statusbar
const uint16_t SBFC = TFT_BLACK;              // Foreground (Text) Color of Statusbar

#define bkColor TFT_BLACK
#define clockColor TFT_YELLOW
#define dateColor TFT_YELLOW

const char* shortMonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* shortDay[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };


/* Y Axis Postion
      0 - 109 : Date and time Area (size 100)
    110 - 229 : Weather Area       (size 120)     
    230 - 239 : Status Bar         (size 10)
*/
#define weatherY  110       // Y Axis position of to start the Weather 
#define weatherTileH  120   // Height of the Weather Area
#define SBY 230             // Y Axis position of to start the StatusBar 
#define SBH 10              // Height of Statusbar

// Compass settings
#define compassX 270
#define compassY 40
#define compassR 35 // Radius

