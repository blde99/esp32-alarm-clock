#include <Arduino.h>
#include "Bounce2.h"      // for button debouncing
#include <Wire.h>         // i2c library
#include <TimeLib.h>      // Time library - this is a clock after all!

#include <WiFi.h>         // Library used primarily for getting the time from the internet

// Includes for OLED screen
#include "images.h"       // Include file containing custom images for OLED screen
#include "SSD1306.h"      // Include library for OLED display

#include "global_vars.h"  // Include global variables
#include "functions.h"    // Include custom functions

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(0);

  pinMode(2, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println("mHZ");
  Serial.println("Setting up...");


  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  esp_sleep_enable_timer_wakeup(SleepTime);
  
   secsTillAlarm();

  
  for (int i=0; i<=5000; i++){
    if ( i % 500 == 0 )
    {
        digitalWrite(2, !digitalRead(2));
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