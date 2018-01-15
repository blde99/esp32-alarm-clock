#include <Arduino.h>
#include "Bounce2.h"      // for button debouncing
#include <Wire.h>         // i2c library
#include <Time.h>      // Time library - this is a clock after all!
#include <Timelib.h>
#include <WiFi.h>         // Library used primarily for getting the time from the internet
#include <Preferences.h>  // Library for storing data that needs to survive a reboot

// Includes for OLED screen
#include "images.h"       // Include file containing custom images for OLED screen
#include "SSD1306.h"      // Include library for OLED display
#include "RTClib.h"       // Library for DS1307 RTC

#include "global_vars.h"  // Include global variables
#include "functions.h"    // Include custom functions

void setup() {
  pinMode(2, OUTPUT);
  pinMode(BTN_ENC_SET_ALARM, INPUT_PULLDOWN);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up...");
  
displayInit();
rtcInit();
//toggleAlarmSet();
getAlarmSettings();

//get_Time();
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//get_Time();


  //DateTime TimeNow = rtc.now();
  //drawTime(TimeNow.hour(),TimeNow.minute());
  


  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  touchAttachInterrupt(T3, callback, Threshold);  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T0, callback, Threshold);  //Setup interrupt on Touch Pad 3 (GPIO4)
  esp_sleep_enable_touchpad_wakeup();             //Configure Touchpad as wakeup source

  if (isAlarmSet){
    uint32_t secsToSleep = secsTillAlarm(rtc.now());
    uint64_t iTimeToSleep = secsToSleep * microSecToSec;
    esp_sleep_enable_timer_wakeup(iTimeToSleep);

    Serial.print("Will sleep for ");
    Serial.print(secsToSleep);
    Serial.println(" seconds.");
  }
  else {
    Serial.println("Will sleep until interrupted by touch or alarm setting...");
  }

  for (int i=0; i<=2000; i++){
    if (digitalRead(BTN_ENC_SET_ALARM)){
      Serial.println("You pushed my button!");
    }
    delay(1);  
  }
  display.displayOff();
  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
}

void loop() {

}