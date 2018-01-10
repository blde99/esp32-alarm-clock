// Definitions for ESP Deep Sleep
#define uS_TO_s_FACTOR 1000000   // Conversion factor for micro seconds to minutes
#define TIME_TO_SLEEP 120        // Time ESP32 will go to sleep (in minutes)
const int SleepTime = TIME_TO_SLEEP * uS_TO_s_FACTOR; // SleepTime will is 5 Seconds

#define DISPLAY_ON_DURATION 2000

// Definitions for touchpad wake up
#define Threshold 40 /* Greater the value, more the sensitivity */
touch_pad_t touchPin;

unsigned int localPort = 2390;      // local port to listen for UDP packets
const char* ntpServerName = "uk.pool.ntp.org";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

const char* ssid     = "SatcomVM";
const char* password = "VirginMedia123";