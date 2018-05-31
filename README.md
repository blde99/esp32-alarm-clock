# ESP32 Alarm Clock

This project is to create an alarm clock using the following components:
- [esp32 thing](https://www.sparkfun.com/products/13907) - I used this because the clock needs to run on battery and this has an integrated charging circuit and connector for a LiPol battery
- [ds3231 RTC module](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)
- [ssd1306 OLED display](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)
- [ky-040 rotary encoder](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)

The project uses the following schematic
![Schematic](/alarm-clock-schematic.png)

### Operation

Once built, the clock will wake from deep sleep in one of two ways:
- Just before the alarm goes off (if set) - In this case the alarm will go off until the user touches the touch pad to acknowledge the alarm
- If the user touches the touch pad - In this case the user will be shown the time for 5 seconds during which time they can use the rotary encoder button to:
  - Single click - Set/unset the alarm
  - Double click - Connect to WiFi to set the time via time server
  - Click and hold - Set the alarm time