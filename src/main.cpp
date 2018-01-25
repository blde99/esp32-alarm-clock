#include <Arduino.h>
#include "Bounce2.h"      // for button debouncing
#include <Wire.h>         // i2c library
#include <Time.h>         // Time library - this is a clock after all!
#include <Timelib.h>
#include "Timezone.h"
#include <WiFi.h>         // Library used primarily for getting the time from the internet
#include <Preferences.h>  // Library for storing data that needs to survive a reboot
#include "ClickEncoder.h"

// Includes for OLED screen
#include "images.h"       // Include file containing custom images for OLED screen
#include "SSD1306.h"      // Include library for OLED display
#include "RTClib.h"       // Library for DS1307 RTC

#include "global_vars.h"  // Include global variables
#include "functions.h"    // Include custom functions

void setup() {
  pinMode(2, OUTPUT);
  pinMode(ENCODER_BTN_SET_ALARM, INPUT_PULLUP);
  pinMode(ENCODER_CW_SET_ALARM, INPUT);
  pinMode(ENCODER_CCW_SET_ALARM, INPUT);
  encoder.setAccelerationEnabled(true);

  oldEncoderPosition = -1;

  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up...");
  Serial.print("Encoder acceleration is ");
  Serial.println((encoder.getAccelerationEnabled()) ? "enabled" : "disabled");

  Serial.println("Init Display"); displayInit();
  Serial.println("Init RTC"); rtcInit();
  Serial.println("Getting alarm settings"); getAlarmSettings();
  //digitalWrite(2, isAlarmSet);

  //print_wakeup_reason();
  //print_wakeup_touchpad();
  switch (print_wakeup_reason()) { 
    case 3: break; // We will add a check time loop in prep for the alarm being triggered because we were woken by timer.
    case 4: showTime(); break; // We were woken by touchpad
  }

  unsigned long starttime, endtime;
  starttime = millis();
  endtime = starttime;

  //for (int i=0; i<=1000; i++){
  while ((endtime - starttime) <=5000) { // do this loop for up to 1000mS
    encoder.service();
    showTime();

    encoderButtonState = encoder.getButton();
    // if (encoderButtonState != 0){
    //   Serial.print("Button: "); Serial.println(encoderButtonState);
    // }
    switch (encoderButtonState) {
      case ClickEncoder::Clicked:          //5
        toggleAlarmSet();
        starttime = millis();
        endtime = starttime;  
        break;

      case ClickEncoder::Held:       //5
        setAlarm();
        starttime = millis();
        endtime = starttime;          
        break;
      case ClickEncoder::DoubleClicked:  //6
        get_Time();
        starttime = millis();
        endtime = starttime;        
        break;
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
  touchAttachInterrupt(T3, callback, TOUCHPIN_SENSITIVITY_THRESHOLD);  //Setup interrupt on Touch Pad 3 (GPIO15)
  esp_sleep_enable_touchpad_wakeup();             //Configure Touchpad as wakeup source  
  display.displayOff();
  Serial.println("Going to sleep now");           //Go to sleep now
  esp_deep_sleep_start();
}

void loop() {}