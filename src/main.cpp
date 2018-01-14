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

  Serial.begin(115200);
  Serial.println();
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println("mHZ");
  Serial.println("Setting up...");
  
displayInit();
rtcInit();
getAlarmSettings();
//get_Time();
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//get_Time();


  //DateTime TimeNow = rtc.now();
  //drawTime(TimeNow.hour(),TimeNow.minute());
  


  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);
  

    //Setup interrupt on Touch Pad 3 (GPIO4)
  //touchAttachInterrupt(T0, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  uint32_t secsToSleep = secsTillAlarm(rtc.now());
  Serial.println(secsToSleep);

  uint64_t iTimeToSleep = secsToSleep * microSecToSec;
  esp_sleep_enable_timer_wakeup(iTimeToSleep);

  Serial.print("Will sleep for ");
  uint32_t low = iTimeToSleep % 0xFFFFFFFF; 
  uint32_t high = (iTimeToSleep >> 32) % 0xFFFFFFFF;

  Serial.print(low);
  Serial.print(".");
  Serial.print(high);   
  Serial.println(" microseconds.");
 
  for (int i=0; i<=5000; i++){
    if ( i % 500 == 0 )
    {
        // digitalWrite(2, !digitalRead(2));
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