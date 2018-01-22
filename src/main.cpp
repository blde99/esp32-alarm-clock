#include <Arduino.h>
#include "Bounce2.h"      // for button debouncing
#include <Wire.h>         // i2c library
#include <Time.h>         // Time library - this is a clock after all!
#include <Timelib.h>
#include "Timezone.h"
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
  pinMode(BTN_TOGGLE_ALARM, INPUT_PULLUP);
  pinMode(ENCODER_BTN_SET_ALARM, INPUT_PULLUP);
  pinMode(ENCODER_CW_SET_ALARM, INPUT_PULLUP);
  pinMode(ENCODER_CCW_SET_ALARM, INPUT_PULLUP);
  alarmtoggleDebouncer.attach(BTN_TOGGLE_ALARM);
  alarmtoggleDebouncer.interval(5); // interval in ms
  alarmtoggleDebouncer.update();
  encoderbtnDebouncer.attach(ENCODER_BTN_SET_ALARM);
  encoderbtnDebouncer.interval(5);
  encoderbtnDebouncer.update();
  encodercwDebouncer.attach(ENCODER_CW_SET_ALARM);
  encodercwDebouncer.interval(10);
  encodercwDebouncer.update();
  encoderccwDebouncer.attach(ENCODER_CCW_SET_ALARM);
  encoderccwDebouncer.interval(20);
  encoderccwDebouncer.update();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up...");
  
  displayInit();
  rtcInit();
  getAlarmSettings();
  digitalWrite(2, isAlarmSet);

  print_wakeup_reason();
  print_wakeup_touchpad();

  unsigned long starttime, endtime;
  starttime = millis();
  endtime = starttime;

  //for (int i=0; i<=1000; i++){
  while ((endtime - starttime) <=5000) { // do this loop for up to 1000mS
    showTime();
    alarmtoggleDebouncer.update();
    encoderbtnDebouncer.update();
    if (encoderbtnDebouncer.rose()) {
      Serial.println("Saw ENCODER button press");
      setAlarm();
      starttime = millis();
      endtime = starttime;      
    }

    if (alarmtoggleDebouncer.rose()) {
      toggleAlarmSet();
      digitalWrite(2, isAlarmSet);
      starttime = millis();
      endtime = starttime;
    }
    endtime = millis();
  }

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
  touchAttachInterrupt(T3, callback, Threshold);  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T0, callback, Threshold);  //Setup interrupt on Touch Pad 0 (GPIO4)
  esp_sleep_enable_touchpad_wakeup();             //Configure Touchpad as wakeup source  
  display.displayOff();
  Serial.println("Going to sleep now");           //Go to sleep now
  esp_deep_sleep_start();
}

void loop() {}