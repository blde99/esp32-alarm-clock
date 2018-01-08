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
#include "functions.h"

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 21, 22); //i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22

void drawTime(String hour, String min) {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(DejaVu_Sans_40);
    display.drawString(64, 10, hour +":"+ min);
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

const char* ssid     = "SatcomVM";
const char* password = "VirginMedia123";
int PrevMin = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up...");
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.print(" / ");
    Serial.println(password);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
         delay(500);
         Serial.print(".");
    }
  //Serial.println(ESP.getCpuFreqMHz());
  //We set our ESP32 to wake up every 1 minute
  esp_sleep_enable_timer_wakeup(SleepTime);
  
  display.init();

  display.flipScreenVertically();

  //display.clear();
  display.setContrast(0);
  configTime(0, 0, "pool.ntp.org");
  setTime(printLocalTime());
  if(PrevMin != minute()) {
    drawTime(twoDigits(hour()), twoDigits(minute()));
    display.display();
  }
  //delay(DISPLAY_ON_DURATION);
  //display.displayOff();
  esp_deep_sleep_start();
}

void loop() {

}