#include <Arduino.h>  // Allows the ESP32 to be coded using Arduino code
#include <Wire.h>     // i2c library
#include <Time.h>     // Time library - this is a clock after all!
#include "Timezone.h" // TimeZone library - used to set BST or GMT
#include <WiFi.h>     // Library used primarily for getting the time from the internet
#include <stdlib.h>
#include <Preferences.h>  // Library for storing data that needs to survive a reboot
#include "ClickEncoder.h" // Library to read inputs from the rotary encoder
#include <Servo.h>        // Library for Servo

// Includes for OLED screen
#include "images.h"  // Include file containing custom images for OLED screen
#include "SSD1306.h" // Include library for OLED display
#include "RTClib.h"  // Library for DS1307 RTC

#include "global_vars.h" // Include global variables
#include "functions.h"   // Include custom functions

void setup()
{
  Serial.begin(115200);         // Setup Serial output
  pinMode(INBUILT_LED, OUTPUT); // Set pin 5 as output - this pin also has the built in LED
  if (!firstStartComplete)      // If we booted from power on, not sleep...
  {
    startSequence(); // ...run the builtin led start sequence
  }
  pinMode(ENCODER_BTN_SET_ALARM, INPUT_PULLUP); // Set pin 34 as input with pullup resistor
  pinMode(ENCODER_CW_SET_ALARM, INPUT);         // Set pin 33 as input
  pinMode(ENCODER_CCW_SET_ALARM, INPUT);        // Set pin 32 as input
  encoder.setAccelerationEnabled(true);         // Enable acceleration on the rotary encoder

  Serial.print("Clock speed: ");                                               // Debug
  Serial.println(ESP.getCpuFreqMHz());                                         // Debug
  Serial.println();                                                            // Debug
  Serial.println("Setting up...");                                             // Debug
  Serial.print("Encoder acceleration is ");                                    // Debug
  Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled"); // Debug

  Serial.println("Init Display");           // Debug
  displayInit();                            // Initialise the OLED display
  Serial.println("Init RTC");               // Debug
  rtcInit();                                // Initialise the DS1307 RTC
  chargeServo.attach(CHARGE_SERVO_PIN, 
                     CHARGE_SERVO_CHANNEL);
  Serial.println("Getting alarm settings"); // Debug
  getAlarmSettings();                       // Retrieve alarm settings from preferences an store them

  String batteryText;
  float vBAT;
  vBAT = ceilf(getBatteryVoltage() * 100) / 100;
  batteryText = String(vBAT) + "V";
  Serial.print("Battery Voltage: ");    // Debug (Prints battery voltage on Serial, this will be displayed somehow)
  Serial.println(batteryText);          // Debug
  
  switch (get_wakeup_reason())
  {        // Get wakeup reason
  case 3:  // We were woken by timer...
    break; // Nothing to do since our default action is to handle being woken by timer.
  case 4:  // We were woken by touchpad...
  
    //oled.setTextAlignment(TEXT_ALIGN_CENTER); // Align text to centre of the OLED display
    //oled.setFont(DejaVu_Sans_40);             // Set the font for the text
    //oled.drawString(64, 10, batteryText);     // Draw the time on the display
    //oled.display();
    //delay(1000);
    showTime(); // ...so show the time on the OLED display
    break;      // Exit Switch
  default:      // We weren't woken by timer or touch...
    break;      // Do Nothing
  }

  unsigned long starttime, endtime; // Declare the variables required for the 5 second loop
  starttime = millis();             // Set the starttime vaiable to milliseconds since boot
  endtime = starttime;              // Set end time to starttime

  while ((endtime - starttime) <= DISPLAY_ON_DURATION)
  {                    // do this loop for up to 5000mS (or value of "DISPLAY_ON_DURATION")
      if (vBAT <= 4.00F)
    { // If vBAT is less than or equal to 4.00V...
      Serial.println("DEBUG: Servo set to 90 degrees");
      chargeServo.write(90); // ...raise charge flag
    }
    else
    { // ...otherwise...
      Serial.println("DEBUG: Servo set to 0 degrees");
      chargeServo.write(0);  // ...lower the charge flag
    }
    
    encoder.service(); // Poll the rotary encoder
    showTime();        // Display the time on the OLED display

    encoderButtonState = encoder.getButton(); // Find out what the rotary encoder button is doing
    switch (encoderButtonState)               // If the encoder button was pressed...
    {                                         // ...figure out what to do
    case ClickEncoder::Clicked:               // If the rotary encoder button was clicked...
      toggleAlarmSet();                       // ...toggle the alarm seting (on or off)
      starttime = millis();                   // Start the 5 second loop again to give the user
      endtime = starttime;                    // a chance to do something else.
      break;

    case ClickEncoder::Held: // If the rotary encoder button was held...
      setAlarm();            // ...start the procedure to set the alarm time.
      starttime = millis();  // Start the 5 second loop again to give the user
      endtime = starttime;   // a chance to do something else.
      break;
    case ClickEncoder::DoubleClicked: // If the rotary encoder button was double clicked...
      get_Time();                     // ...start the procedure to set the clock time from NTP.
      starttime = millis();           // Start the 5 second loop again to give the user
      endtime = starttime;            // a chance to do something else.
      break;
    }

    if (timeCheck() && !alarmAcknowledged)
    {                       // If current time matches alarm time and the alarm has NOT been acknowledged, trigger the alarm.
      triggerAlarm();       // Alarm has been triggered.
      starttime = millis(); // Start the 5 second loop again to give the user
      endtime = starttime;  // a chance to do something else.
    }
    endtime = millis(); // Set the endtime variable
  }

  if (isAlarmSet)
  {                                                  // If the alarm is set...
    uint64_t secsToSleep = secsTillAlarm(rtc.now()); // ...figure out how long (in seconds) till the next alarm
    uint64_t iTimeToSleep = secsToSleep * 1000000;   // Convert that to microseconds...
    esp_sleep_enable_timer_wakeup(iTimeToSleep);     // ...and set the ESP32 sleep timer to that number
    char str[21];

    Serial.print("Will sleep for ");     // Debug
    Serial.printf("%llu", secsToSleep);  // Debug
    Serial.print(" seconds or ");        // Debug
    Serial.printf("%llu", iTimeToSleep); // Debug
    Serial.println(" microseconds.");    // Debug
  }
  else
  {
    Serial.println("Will sleep until interrupted by touch or alarm setting..."); // Debug
  }
  touchAttachInterrupt(TOUCH_PIN,                       // Setup interrupt on Touch Pad 3 (GPIO15)
                       callback,                        // When woken by touch, use the dummy callback variable
                       TOUCHPIN_SENSITIVITY_THRESHOLD); // Use the sensitivity threshold value defined in global_vars.h
  esp_sleep_enable_touchpad_wakeup();                   // Configure Touchpad as wakeup source
  oled.displayOff();                                    // Turn off the OLED display
  Serial.println("Going to sleep now");                 // Debug
  esp_deep_sleep_start();                               // Go to sleep now
}

void loop() {} // Main loop never gets executed due to deep sleep.