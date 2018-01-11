#include <Arduino.h>
#include "Bounce2.h" // for button debouncing
#include <Wire.h>  // i2c library
#include <TimeLib.h>
#include <Time.h>

#include <WiFi.h>

//#include <ESP8266WiFi.h>
//#include "ESP8266WebServer.h"

// Includes for OLED screen
#include "images.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#include "global_vars.h"

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 21, 22); //i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22

#include "functions.h"

int PrevMin = 0;

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
  
  
  for (int i=0; i<=5000; i++){
    if ( i % 500 == 0 )
    {
        digitalWrite(2, !digitalRead(2));
    }
    delay(1);  
  }
  secsTillAlarm(hour(),minute(),second(),8,0,0);
  display.displayOff();
  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
}

void loop() {

}