// Definitions for ESP Deep Sleep
#define uS_TO_s_FACTOR 1000000   // Conversion factor for micro seconds to minutes
#define TIME_TO_SLEEP 5        // Time ESP32 will go to sleep (in minutes)
const int SleepTime = TIME_TO_SLEEP * uS_TO_s_FACTOR; // SleepTime will is 5 Seconds

#define DISPLAY_ON_DURATION 2000

char ssid[] = "SatcomVM";  //  your network SSID (name)
char pass[] = "VirginMedia123";       // your network password
unsigned int localPort = 2390;      // local port to listen for UDP packets
const char* ntpServerName = "uk.pool.ntp.org";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
