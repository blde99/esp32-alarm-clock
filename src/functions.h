// Function to get the Alarm settings from non-volatile memory
// Parameters:
// None
// Returns:
// None
void getAlarmSettings () {
  preferences.begin("alarmclock", false);           // Open preferences
  isAlarmSet = preferences.getBool("alarmSet");     // Get alarm set status
  hourAlarm = preferences.getUInt("hourAlarm", 0);  // Get alarm hour and place in a global variable
  minAlarm = preferences.getUInt("minAlarm", 0);    // Get alarm minute and place in a global variable
  secAlarm = preferences.getUInt("secAlarm", 0);    // Get alarm second and place in a global variable (this will always be zero)
  preferences.end();                                // Close preferences
}

// Function to initialise the OLED display
// Parameters:
// None
// Returns:
// None
void displayInit () {
  display.init();                                   // Initialise the display
  display.flipScreenVertically();                   // Flip the display 180 degrees
  display.setContrast(0);                           // Set a really low contrast
}

// Function to initialise the DS1307 RTC module
// Parameters:
// None
// Returns:
// None
void rtcInit () {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC module!");    // Print fail message if RTC can't be found
  while (1);                                        // Loop forever
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC module is not running!");   // Print fail message if RTC can be found but the time is not set
  }
}

// Function to calculate the difference between the time now and the NEXT alarm
// Parameters:
// DateTime t - Intended to be the time now when passed in
// Returns:
// secondsTillAlarm - this is the amount of time in seconds between "t" and the NEXT alarm
uint64_t secsTillAlarm(DateTime t) {
  int secondsTillAlarm = 0;                                             // Initialise the return variable

  int hoursLeft = hourAlarm - t.hour();                                 // Calculate the difference in hours between now and the alarm
  if (hoursLeft < 0) { hoursLeft += 24; }                               // Add 24 hours if we are past the alarm hour
  int minsLeft = minAlarm - t.minute();                                 // Calculate the difference in minutes between now and the alarm
  if (minsLeft < 0) { minsLeft += 60; }                                 // Add 60 minutes if we are past the alarm minute
  int secsLeft = secAlarm - t.second();                                 // Calculate the difference in seconds between now and the alarm
  if (secsLeft < 0) { secsLeft += 60; }                                 // Add 60 seconds if we are past the alarm seconds

  secondsTillAlarm = (hoursLeft * 3600) + (minsLeft * 60) + secsLeft;   // Convert it all to seconds...

  return secondsTillAlarm - 70;                                         // ... and return it
}

// utility function for digital clock display: prints leading 0
// Parameters:
// int digits - digit to be converted to string and if necessary add a leading 0
// Returns:
// paddedDigits - String to be printed.
String twoDigits(int digits){
  String paddedDigits;                                // Initialise the return variable
  if(digits < 10) {
    paddedDigits = '0'+String(digits);                // If the "digits" parameter is less then 10, add a "0" before the digit
  }
  else {
    paddedDigits = String(digits);                    // If the "digits" parameter is more than 10, return the orinal number as a string
  }
  return paddedDigits;                                // Return the new string  
}

// Function to get local time
// Parameters:
// none
// Returns:
// timeSinceEpoch - Seconds since Epoch if successful
// 0 - Returns this if failed
time_t printLocalTime()
{
    struct tm timeinfo;                               // Create struct to place time into
    if(!getLocalTime(&timeinfo)){                     // Get the time and place it into the timeinfo variable
        Serial.println("Failed to obtain time");      // Print to serial if we fail...
        return 0;                                     // ...and return 0
    }
    time_t timeSinceEpoch = mktime(&timeinfo);        // Convert timeinfo variable to epoch time and put into the "timeSinceEpoch" variable
    return timeSinceEpoch;                            // Return the "timeSinceEpoch" variable
}

// Function to display the time/alarm on the OLED display
// Parameters:
// int hourtoPrint - The hour portion of the time.  This is use in conjunction with the twoDigits() function
// int mintoPrint - The minute portion of the time.  This is use in conjunction with the twoDigits() function
// bool alarmSetmode - If the alarm is being set, then this is "true" which in turn will draw the cog icon on the screen
// Returns:
// None
void drawTime(int hourtoPrint, int mintoPrint, bool alarmSetMode = false) {
    display.clear();                                                                              // Clear the display
    
    if (isAlarmSet){                                                                              // If the alarm is set...
      display.drawXbm(110, 0, alarm_on_small_width, alarm_on_small_height, alarm_on_small_bits);  // ...display the alarm on icon top right of OLED
    }
    if (alarmSetMode) {                                                                           // If we are setting the alarm time...
      display.drawXbm(0, 0, cog_small_width, cog_small_height, cog_small_bits);                   // ...display the cog icon top right of OLED
    }
    
    String timeToDraw = String(twoDigits(hourtoPrint)) + ":" + String(twoDigits(mintoPrint));     // Set the "timeToDraw" variable
    display.setTextAlignment(TEXT_ALIGN_CENTER);                                                  // Align text to centre of the OLED display
    display.setFont(DejaVu_Sans_40);                                                              // Set the font for the text
    display.drawString(64, 10, timeToDraw);                                                       // Draw the time on the display
    display.display(); 
}

// Function to draw the WiFi logo on the OLED display
// Parameters:
// none
// Returns:
// none
void drawWiFiImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

// Function to draw the Alarm on image on the OLED display when the alarm state is toggled
// Parameters:
// none
// Returns:
// none
void drawAlarmOnImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(40, 8, alarm_on_width, alarm_on_height, alarm_on_bits);
}

// Function to draw the Alarm off image on the OLED display when the alarm state is toggled
// Parameters:
// none
// Returns:
// none
void drawAlarmOffImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(40, 8, alarm_off_width, alarm_off_height, alarm_off_bits);
}

// Function to draw the error image on the OLED if the clock failed to set the time by NTP
// Parameters:
// none
// Returns:
// none
void drawErrorImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.clear();
    display.drawXbm(40, 8, error_width, error_height, error_bits);
    display.display();
}

// Function to draw the success image on the OLED if the clock succeeded in setting the time by NTP
// Parameters:
// none
// Returns:
// none
void drawSuccessImage() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.clear();
    display.drawXbm(40, 8, success_width, success_height, success_bits);
    display.display();
}

// Function to connect and get the time over WiFi
// Parameters:
// none
// Returns:
// none
void get_Time(){
  time_t returnedTime;                                            // Variable the is filled by the printLocalTime() function

  preferences.begin("alarmclock", false);                         // Open preferences
  ssid = preferences.getString("ssid-work");                      // Get the WiFi SSID stored previously
  password = preferences.getString("password-work");              // Get the WiFi password stored previously
  preferences.end();                                              // Close preferences
  
  int counter = 0;                                                // Initialise a counter
  Serial.print("Connecting to ");                                 // Debug
  Serial.print(ssid);                                             // Debug

  WiFi.begin(ssid.c_str(), password.c_str());                     // Connect to WiFi using the details gathered earlier
  display.clear();                                                // Clear the OLED display
  drawWiFiImage();                                                // Show the WiFi logo to indicate we are trying to connect to WiFi
  display.display();                                              // Update the OLED display
  while ((WiFi.status() != WL_CONNECTED) && (counter <= 10)) {    // If we haven't connected to WiFi yet...
    delay(500);                                                   // Show dots in the Serial monitor for 5 secs
    Serial.print(".");
    counter++;
  }
  if(WiFi.status() == WL_CONNECTED) {                             // If we successfully connect to WiFi
    Serial.println("Connected!");
    configTime(0, 0, ntpServerName);                              // Set the GMT offset, daylight savings and NTP server
    returnedTime = printLocalTime();                              // Get the time and fill the "returnedTime" variable
    if (returnedTime != 0) {                                      // If we were successful getting the time...
      rtc.adjust(returnedTime);                                   // ...set the DS1307 RTC module
      drawSuccessImage();                                         // Draw the success icon on the OLED display
      delay(500); 
    }
    else {
      drawErrorImage();                                           // If we failed to get the time, draw the error icon on the OLED display
      delay(500);      
    }
  }
  else {
    Serial.println("Failed to connect!");                         // If we failed to connect to WiFi, draw the error icon on the OLED display
    drawErrorImage();
    delay(500);
  }
}

// Function to show the time on the OLED display
// Parameters:
// none
// Returns:
// none
void showTime () {
  rtcTime = rtc.now();                        // Get the time from the RTC module
  drawTime(rtcTime.hour(),rtcTime.minute());  // Show the time on the OLED display
}

// Function to toggle the alarm set status
// Parameters:
// none
// Returns:
// none
void toggleAlarmSet () {
  preferences.begin("alarmclock", false);                                   // Open preferences
  preferences.putBool("alarmSet", !preferences.getBool("alarmSet", true));  // Write the opposite status to the preference to what is read
  isAlarmSet = preferences.getBool("alarmSet");                             // Fill the "isAlarmSet" variable
  preferences.end();                                                        // Close the preferences
  
  display.clear();                                                          // Clear the OLED display
  display.setTextAlignment(TEXT_ALIGN_CENTER);                              // Align text to centre
  display.setFont(ArialMT_Plain_24);                                        // Set the font
  if (isAlarmSet) {                                                         // If the alarm is set...
    drawAlarmOnImage();                                                     // ...Draw the Alarm on icon
    display.display();                                                      // Update the display
    delay(500);
  }
  else {                                                                    // If the alarm is not set...
    drawAlarmOffImage();                                                    // ...Draw the Alarm on icon
    display.display();                                                      // Update the display
    delay(500);
  }
}

// Function to get the reason for esp32 wakeup
// Parameters:
// none
// Returns:
// an integer in the range 1..5 - we are only interested in 3 & 4 (Timer and Touchpad)
int get_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); return 1; break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); return 2; break;
    case 3  : Serial.println("Wakeup caused by timer"); return 3; break;
    case 4  : Serial.println("Wakeup caused by touchpad"); return 4; break;
    case 5  : Serial.println("Wakeup caused by ULP program"); return 5; break;
    default : Serial.println("Wakeup was not caused by deep sleep"); return 0; break;
  }
}

// dummy callback function for ESP32 deepsleep wakeup
void callback(){}

// Function to set the alarm time
// Parameters:
// none
// Returns:
// none
void setAlarm () {
  bool alarmSetComplete = false;                                            // Create vaiable used to trap us into a loop
  int setFunction = 0;                                                      // setFunction will be 0, 1 or 2 (Hours, minutes, finished)

  while (!alarmSetComplete) {                                               
    encoder.service();                                                      // Get values from the rotary encoder
    drawTime(hourAlarm, minAlarm, true);                                    // Update the alarm time on the OLED display

    encoderButtonState = encoder.getButton();                               // Get the rotary encoder button status
    if (encoderButtonState == ClickEncoder::Clicked) { setFunction++; }     // If the button was clicked, start the next function
    switch (setFunction) {
      case 0:                                                               // If setFunction is 0, we are setting the hour for the alarm
        hourAlarm += encoder.getValue();                                    // Update the "hourAlarm" variable by incrementing or decrementing from encoder
        if (hourAlarm > 23) {                                               // If "hourAlarm" is 24 or more...
          hourAlarm = 0;                                                    // ...set "hourAlarm" to 0 to start again
        }
        if (hourAlarm < 0) {                                                // If "hourAlarm" is -1 or less...
          hourAlarm = 23;                                                   // ...set "hourAlarm" to 24 to start again
        }        
        break;
      case 1:                                                               // If setFunction is 0, we are setting the minute for the alarm
        minAlarm += encoder.getValue();                                     // Update the "minAlarm" variable by incrementing or decrementing from encoder
        if (minAlarm > 59) {                                                // If "minAlarm" is 60 or more...
          minAlarm = 0;                                                     // ...set "hourAlarm" to 0 to start again
        }
        if (minAlarm < 0) {                                                 // If "minAlarm" is -1 or less...
          minAlarm = 59;                                                    // ...set "minAlarm" to 59 to start again
        }
        break;
      case 2:                                                               // If setFunction is 0, we have finished setting the alarm
        preferences.begin("alarmclock", false);                             // Open preferences
        preferences.putUInt("hourAlarm", hourAlarm);                        // Write the hour value to the preferences
        preferences.putUInt("minAlarm", minAlarm);                          // Write the minute value to the preferences
        preferences.putUInt("secAlarm", 0);                                 // Write the seconds value to the preferences
        preferences.end();                                                  // Close perefernces
        alarmSetComplete = true;                                            // Set "alarmSetComplete" to true to break out of the loop
        break;
    }
  }
  
}