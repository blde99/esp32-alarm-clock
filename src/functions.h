unsigned int secsTillAlarm( ) {
  time_t t = now(); // Store the current time in time 
  Serial.println(t);
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
    //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.print("Time since epoch: ");
    Serial.println(timeSinceEpoch);
    return timeSinceEpoch;
}

void drawTime(String hourtoPrint, String mintoPrint) {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.clear();
    String timeToDraw = hourtoPrint +":"+ mintoPrint;
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(DejaVu_Sans_40);
    display.drawString(64, 10, timeToDraw);
    display.display();
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void get_Time(){
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print("...");

  WiFi.begin(ssid, password);
  drawImageDemo();
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("!");
  configTime(0, 0, ntpServerName);
  setTime(printLocalTime());
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){
  touch_pad_t pin;
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); 
              get_Time();
              drawTime(twoDigits(hour()), twoDigits(minute()));
              break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void callback(){
  //placeholder callback function
}