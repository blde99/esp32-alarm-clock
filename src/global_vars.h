// Definitions for ESP Deep Sleep
// #define uS_TO_s_FACTOR 1000000   // Conversion factor for micro seconds to minutes
// #define TIME_TO_SLEEP 120        // Time ESP32 will go to sleep (in minutes)
// const int SleepTime = TIME_TO_SLEEP * uS_TO_s_FACTOR; // SleepTime will is 5 Seconds
#define DISPLAY_ON_DURATION 2000
#define ENCODER_BTN_SET_ALARM 34
#define ENCODER_CW_SET_ALARM 33
#define ENCODER_CCW_SET_ALARM 32
#define ENCODER_STEPS 4

int16_t oldEncoderPosition, encoderPosition;
uint8_t encoderButtonState;


// Definitions for touchpad wake up
#define TOUCHPIN_SENSITIVITY_THRESHOLD 40 /* Greater the value, more the sensitivity */
touch_pad_t touchPin;

const char* ntpServerName = "uk.pool.ntp.org";

//const char* ssid     = "AndroidAP-24630";
//const char* password = "Plonker12";

String ssid;
String password;

// Initialize devices
SSD1306  display(0x3c, 21, 22); //i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22
RTC_DS1307 rtc; // RTC
DateTime rtcTime;

const uint32_t microSecToSec = 1000000;

// Initialize preferences
Preferences preferences;

bool isAlarmSet;
int hourAlarm;
int minAlarm;
int secAlarm;
ClickEncoder encoder(ENCODER_CCW_SET_ALARM, ENCODER_CW_SET_ALARM, ENCODER_BTN_SET_ALARM, ENCODER_STEPS);