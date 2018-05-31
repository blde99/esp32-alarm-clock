# ESP32 Alarm Clock

This project is to create an alarm clock using the following components:
- [ESP32 thing](https://www.sparkfun.com/products/13907) - I used this because the clock needs to run on battery and this has an integrated charging circuit and connector for a LiPol battery
- [Single cell lithium polymer battery](https://coolcomponents.co.uk/products/lithium-polymer-battery-2000mah)
- [DS3231 RTC module](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)
- [SSD1306 OLED display](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)
- [KY-040 rotary encoder with button](https://www.amazon.co.uk/SODIAL-DS3231-AT24C32-Precision-Arduino/dp/B00K67X496/)
- Piezo buzzer (can't wake someone up without noise, right?)

The clock uses the following schematic
![Schematic](/alarm-clock-schematic.png)

### Information

Because the clock runs on the LiPo battery, it spends most of it life in deep sleep and only wakes up on demand or if the alarm is about to go off.

I chose the Sparkfun esp32 thing because of it's low current draw while in deep sleep.  The OLED display also only draws microamps even while on. Because of this, the clock can run for months on battery.  I know that the I could power this using the USB connectory, but there are no plug sockets near where the clock will live, so I needed to power it using battery.

### Operation

Once built, the clock will wake from deep sleep in one of two ways:
- Just before the alarm goes off (if set) - In this case the alarm will go off until the user touches the touch pad to acknowledge the alarm
- If the user touches the touch pad - In this case the user will be shown the time for 5 seconds during which time they can use the rotary encoder button to:
  - Single click - Set/unset the alarm
  - Double click - Connect to WiFi to set the time via time server
  - Click and hold - Set the alarm time