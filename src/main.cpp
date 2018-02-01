#include <Arduino.h>      // Allows the ESP32 to be coded using Arduino code
#include "Bounce2.h"      // for button debouncing
#include <Wire.h>         // i2c library
#include <Time.h>         // Time library - this is a clock after all!
#include "Timezone.h"     // TimeZone library - used to set BST or GMT
#include <WiFi.h>         // Library used primarily for getting the time from the internet
#include <Preferences.h>  // Library for storing data that needs to survive a reboot
#include "ClickEncoder.h" // Library to read inputs from the rotary encoder

// Includes for OLED screen
#include "images.h"       // Include file containing custom images for OLED screen
#include "SSD1306.h"      // Include library for OLED display
#include "RTClib.h"       // Library for DS1307 RTC

#include "global_vars.h"  // Include global variables
#include "functions.h"    // Include custom functions

void setup() {
  pinMode(2, OUTPUT);                                       // Set pin 2 as output - this pin also has the built in LED
  pinMode(ENCODER_BTN_SET_ALARM, INPUT_PULLUP);             // Set pin 34 as input with pullup resistor
  pinMode(ENCODER_CW_SET_ALARM, INPUT);                     // Set pin 33 as input
  pinMode(ENCODER_CCW_SET_ALARM, INPUT);                    // Set pin 32 as input
  encoder.setAccelerationEnabled(true);                     // Enable acceleration on the rotary encoder

  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up...");
  Serial.print("Encoder acceleration is ");
  Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");

  Serial.println("Init Display"); displayInit();
  Serial.println("Init RTC"); rtcInit();
  Serial.println("Getting alarm settings"); getAlarmSettings();
  
  switch (get_wakeup_reason()) {                              // Get wakeup reason
    case 3: break;                                            // We will add a check time loop in prep for the alarm being triggered because we were woken by timer.
    case 4: showTime(); break;                                // We were woken by touchpad, so show the time on the OLED display
  }

  unsigned long starttime, endtime;                           // Declare the variables required for the 5 second loop
  starttime = millis();                                       // Set the starttime vaiable to milliseconds since boot
  endtime = starttime;                                        // Set end time to starttime

    while ((endtime - starttime) <= DISPLAY_ON_DURATION) {    // do this loop for up to 5000mS (or value of "DISPLAY_ON_DURATION")
    encoder.service();                                        // Poll the rotary encoder
    showTime();                                               // Display the time on the OLED display

    encoderButtonState = encoder.getButton();                 // Find out what the rotary encoder button is doing
    switch (encoderButtonState) {
      case ClickEncoder::Clicked:                             // If the rotary encoder button was clicked...
        toggleAlarmSet();                                     // ...toggle the alarm seting (on or off)
        starttime = millis();                                 // Start the 5 second loop again to give the user
        endtime = starttime;                                  // a chance to do something else.
        break;

      case ClickEncoder::Held:                                // If the rotary encoder button was held...
        setAlarm();                                           // ...start the procedure to set the alarm time.
        starttime = millis();                                 // Start the 5 second loop again to give the user
        endtime = starttime;                                  // a chance to do something else.
        break;
      case ClickEncoder::DoubleClicked:                       // If the rotary encoder button was double clicked...
        get_Time();                                           // ...start the procedure to set the clock time from NTP.
        starttime = millis();                                 // Start the 5 second loop again to give the user
        endtime = starttime;                                  // a chance to do something else.        
        break;
    }

    if (timeCheck() && !alarmAcknowledged) {
      // Alarm has been triggered.
      triggerAlarm();

      starttime = millis();                                 // Start the 5 second loop again to give the user
      endtime = starttime;                                  // a chance to do something else.
    }
    //Serial.println(touchRead(TOUCH_PIN));
    endtime = millis();                                       // Set the endtime variable
  }

  if (isAlarmSet){                                            // If the alarm is set...
    uint32_t secsToSleep = secsTillAlarm(rtc.now());          // ...figure out how long (in seconds) till the next alarm
    uint64_t iTimeToSleep = secsToSleep * microSecToSec;      // Convert that to microseconds...
    esp_sleep_enable_timer_wakeup(iTimeToSleep);              // ...and set the ESP32 sleep timer to that number

    Serial.print("Will sleep for ");
    Serial.print(secsToSleep);
    Serial.println(" seconds.");
  }
  else {
    Serial.println("Will sleep until interrupted by touch or alarm setting...");
  }
  touchAttachInterrupt(TOUCH_PIN,                                    // Setup interrupt on Touch Pad 3 (GPIO15)
                       callback,                              // When woken by touch, use the dummy callback variable
                       TOUCHPIN_SENSITIVITY_THRESHOLD);       // Use the sensitivity threshold value defined in global_vars.h
  esp_sleep_enable_touchpad_wakeup();                         // Configure Touchpad as wakeup source  
  display.displayOff();                                       // Turn off the OLED display
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();                                     // Go to sleep now
}

void loop() {}                                                // Main loop never gets executed due to deep sleep.