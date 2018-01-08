#include <Arduino.h>
#include "Bounce2.h" // for button debouncing
#include <Wire.h>  // i2c library

// Includes for OLED screen
#include "images.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 21, 22); //i2c address for SSD1306 is 0x3c and is connected to pins D21 and D22

#define uS_TO_m_FACTOR 60000000   // Conversion factor for micro seconds to minutes
#define uS_TO_s_FACTOR 1000000   // Conversion factor for micro seconds to minutes
#define TIME_TO_SLEEP 5        // Time ESP32 will go to sleep (in minutes)
#define DISPLAY_ON_DURATION 2000

const int SleepTime = TIME_TO_SLEEP * uS_TO_s_FACTOR;
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;
void drawTimeDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(DejaVu_Sans_40);
    display.drawString(64, 10, "08:00");
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

Demo demos[] = {drawTimeDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println(ESP.getCpuFreqMHz());
  //We set our ESP32 to wake up every 1 minute
  esp_sleep_enable_timer_wakeup(SleepTime);
  
  display.init();

  display.flipScreenVertically();

  //display.clear();
  display.setContrast(0);
  drawTimeDemo();
  display.display();
  delay(DISPLAY_ON_DURATION);
  display.displayOff();
  esp_deep_sleep_start();
}

void loop() {

}