void getAlarmSettings () {
  Serial.println("Getting alarm settings");
  preferences.begin("alarmclock", false);
  // preferences.putUInt("hourAlarm", 15);
  // preferences.putUInt("minAlarm", 0);
  // preferences.putUInt("secAlarm", 0);
  isAlarmSet = preferences.getBool("alarmSet");
  hourAlarm = preferences.getUInt("hourAlarm", 0);
  minAlarm = preferences.getUInt("minAlarm", 0);
  secAlarm = preferences.getUInt("secAlarm", 0);
  preferences.end();
}
void displayInit () {
  Serial.println("Init display...");
  //Initialize display
  display.init();
  display.flipScreenVertically();
  display.setContrast(0);
}

void rtcInit () {
  Serial.println("Init RTC...");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC module!");
  while (1);
  }
  if (!rtc.isrunning()) {
  Serial.println("RTC module is not running!");
    // following line sets the RTC to the date & time this sketch was compiled
     //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
  }
}

uint64_t secsTillAlarm(DateTime t) {
  int secondsTillAlarm = 0;

  int hoursLeft = hourAlarm - t.hour();
  if (hoursLeft < 0) { hoursLeft += 24; }
  int minsLeft = minAlarm - t.minute();
  if (minsLeft < 0) { minsLeft += 60; }
  int secsLeft = secAlarm - t.second();
  if (secsLeft < 0) { secsLeft += 60; }

  secondsTillAlarm = (hoursLeft * 3600) + (minsLeft * 60) + secsLeft;

  return secondsTillAlarm - 70;  
}

// utility function for digital clock display: prints leading 0
String twoDigits(int digits){
  if(digits < 10) {
    String i = '0'+String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}

time_t printLocalTime()
{
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return 0;
    }
    time_t timeSinceEpoch = mktime(&timeinfo);
    Serial.print("Time since epoch: ");
    Serial.println(timeSinceEpoch);
    return timeSinceEpoch;
}

void drawTime(int hourtoPrint, int mintoPrint) {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.clear();
    
    if (isAlarmSet){
      display.drawXbm(110, 0, alarm_on_small_width, alarm_on_small_height, alarm_on_small_bits);
    }
    
    String timeToDraw = String(twoDigits(hourtoPrint)) + ":" + String(twoDigits(mintoPrint));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(DejaVu_Sans_40);
    display.drawString(64, 10, timeToDraw);
    display.display();
}

void drawWiFiImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void drawAlarmOnImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(40, 8, alarm_on_width, alarm_on_height, alarm_on_bits);
}

void drawAlarmOffImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(40, 8, alarm_off_width, alarm_off_height, alarm_off_bits);
}

void get_Time(){
  preferences.begin("alarmclock", false);
  ssid = preferences.getString("ssid-home");
  password = preferences.getString("password-home");
  preferences.end();
  
  int counter = 0;
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());
  drawWiFiImage();
  display.display();
  while ((WiFi.status() != WL_CONNECTED) && (counter <= 10)) {
    delay(500);
    Serial.print(".");
    counter++;
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    configTime(0, 0, ntpServerName);
    //setTime(printLocalTime());
    rtc.adjust(printLocalTime());
    //Serial.println(DateTime(printLocalTime())):
  }
  else {
    Serial.println("Failed to connect!");
  }
}

void showTime () {
  rtcTime = rtc.now(); // Get the time from the RTC module
  //Serial.print(String(rtcTime.hour()));
  //Serial.print(":");
  //Serial.println(String(rtcTime.minute()));
  drawTime(rtcTime.hour(),rtcTime.minute());
}

void toggleAlarmSet () {
  preferences.begin("alarmclock", false);
  //preferences.putBool("alarmSet", true);
  preferences.putBool("alarmSet", !preferences.getBool("alarmSet", true));
  isAlarmSet = preferences.getBool("alarmSet");
  preferences.end();
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  if (isAlarmSet) {
    digitalWrite(2, isAlarmSet);
    Serial.println("Alarm is SET!");
    // display.drawString(64, 10, "SET!");
    display.clear();
    drawAlarmOnImage();
    display.display();
    delay(500);
  }
  else {
    digitalWrite(2, isAlarmSet);
    Serial.println("Alarm is NOT SET!");
    // display.drawString(64, 10, "NOT SET!");
    display.clear();
    drawAlarmOffImage();
    display.display();
    delay(500);
  }
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); digitalWrite(2, HIGH); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void callback(){}
                
/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){
  
  touch_pad_t pin;
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); get_Time(); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); showTime(); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void setAlarm () {
  bool alarmSetComplete = false;

  while (!alarmSetComplete) {
    encoderbtnDebouncer.update();
    encodercwDebouncer.update();
    encoderccwDebouncer.update();
    drawTime(hourAlarm,minAlarm);

    if (encodercwDebouncer.fell()) {
      Serial.println("Knob turned CW");      
    }
    else if (encoderccwDebouncer.fell()) {
      Serial.println("Knob turned CCW");
    }
    if (encoderbtnDebouncer.rose()) {
      alarmSetComplete = true;
    }
    delay(20);
  }
}