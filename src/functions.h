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
        return NULL;
    }
    time_t timeSinceEpoch = mktime(&timeinfo);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.println(timeSinceEpoch);
    return timeSinceEpoch;
}