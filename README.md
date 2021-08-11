## Web Tachometer
ESP32 based web monitoring of RPM via hall effect sensors

### Bill of Materials
* [ESP32 Development Board](https://www.adafruit.com/product/3269)
* [A3144 Hall Effet Sensors (2)] (https://www.amazon.com/Gikfun-AH3144E-Magnetic-Detector-Arduino/dp/B07QS6PN3B)
* [SSD1306 128x64 OLED Display](https://www.amazon.com/gp/product/B072Q2X2LL)

### Wiring/Construction
* Connect the A3144 data pins to GPIO 17 and 18 (internal pullup is used, so no need to add one)
* Connect the SSD1306 SDA pin to GPIO 21 and SCL pin to GPIO 22

### Software/Firmware Installation
* Edit auth.h to connect to a local wifi network
* Upload to an ESP development board via the Arduino IDE

-- Yuri - Aug 2021
