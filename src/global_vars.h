// Defines
#define TOUCH_PIN T3                      // Touch pin T3 is actually GPIO15
#define DISPLAY_ON_DURATION 5000          // Time in ms that the display will be on after an operation
#define TOUCHPIN_SENSITIVITY_THRESHOLD 60 // Set the touchpad sensitivity for deep sleep wakeup
#define ENCODER_BTN_SET_ALARM 34          // ESP32 pin for the rotary encoder button
#define ENCODER_CW_SET_ALARM 33           // ESP32 pin for the rotary encoder clockwise trigger
#define ENCODER_CCW_SET_ALARM 32          // ESP32 pin for the rotary encoder counter clockwise trigger
#define ENCODER_STEPS 4                   // Used to debound the encoder turns - I think!
#define ALARM_BUZZER_PIN 12               // Pin used for the piezo buzzer
#define ALARM_BUZZER_FREQUENCY 2000       // Frequency in Hz for buzzer
#define ALARM_BUZZER_CHANNEL 0            // Channel number for buzzer
#define ALARM_BUZZER_RESOLUTION 8         // Resolution for buzzer
#define ALARM_BUZZER_DUTYCYCLE 50         // Duty Cycle for the buzzer - up to 255.  Can be used for adjusting the volume of the buzzer
#define INBUILT_LED 5                     // Built in LED
#define BATTERY_READ_PIN 35               // Pin used to read battery voltage
#define CHARGE_SERVO_PIN 23               // Pin for Servo
#define CHARGE_SERVO_CHANNEL 1

// Global variables
uint8_t encoderButtonState;                    // Used to figure out if the rotary encoder button was clicked, held, etc.
const char *ntpServerName = "uk.pool.ntp.org"; // Set NTP server name
String ssid;                                   // SSID for WiFi connection
String password;                               // SSID for WiFi connection

// Initialize devices
SSD1306 oled(0x3c, 21, 22); // i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22
RTC_DS1307 rtc;                // DS1307 RTC
DateTime rtcTime;              // Used to store the curent date and time
ClickEncoder encoder(
    ENCODER_CCW_SET_ALARM, // CCW esp32 pin
    ENCODER_CW_SET_ALARM,  // CW esp32 pin
    ENCODER_BTN_SET_ALARM, // Button esp32 pin
    ENCODER_STEPS);        // Initialise the rotary enoder

Preferences preferences;                // Initialize preferences used to store variables that survive poweroff

bool isAlarmSet;                              // Variable that signifies whether or not the alarm is set
int hourAlarm;                                // Store the alarm hour
int minAlarm;                                 // Store the alarm minute
int secAlarm;                                 // Store the alarm second
RTC_DATA_ATTR bool alarmAcknowledged = false; // If the alarm has been acknowledged then this will be true
RTC_DATA_ATTR bool firstStartComplete = false; // Will be set to true by startSequence() after start sequence has been completed.

TimeChangeRule BST = {"BST", Last, Sun, Mar, 2, 60}; //Central European Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};  //Central European Standard Time
Timezone UK(BST, GMT);

Servo chargeServo;
RTC_DATA_ATTR bool batteryChargeRequired = false;
RTC_DATA_ATTR bool batteryChargeFlagRaised = false;