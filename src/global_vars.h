// Definitions for ESP Deep Sleep
#define uS_TO_s_FACTOR 1000000   // Conversion factor for micro seconds to minutes
#define TIME_TO_SLEEP 120        // Time ESP32 will go to sleep (in minutes)
const int SleepTime = TIME_TO_SLEEP * uS_TO_s_FACTOR; // SleepTime will is 5 Seconds

#define DISPLAY_ON_DURATION 2000

// Definitions for touchpad wake up
#define Threshold 40 /* Greater the value, more the sensitivity */
touch_pad_t touchPin;

const char* ntpServerName = "uk.pool.ntp.org";

//const char* ssid     = "SatcomVM";
//const char* password = "VirginMedia123";
const char* ssid     = "VM6584364";
const char* password = "ngw7pxcbPrk9";

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 21, 22); //i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22