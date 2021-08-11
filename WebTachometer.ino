/*********************************************************************************
 * Web Tachometer
 * 
 * Web served temperature, humidity, and light level monitoring/logging
 * 
 * Hardware used: ESP32 Dev Board (see note below)
 *                A3411 Hall Effect Sensor(s)
 *                SSD1306 128*64 OLED
 *                
 *                
 *                Note: This sketch will not comppile for an ESP8266 or Arduino
 *                      variant.  It *should* be trivial to refactor for other
 *                      boards, but code modification would be necessary, since
 *                      this sketch relies on ESP32-centric functions and dependencies.
 *                
 * Dependencies: ESPAsyncWebServer - https://github.com/me-no-dev/ESPAsyncWebServer
 *
 * -- Yuri - Aug 2021
 * 
 * Based on code by Rui Santos - https://RandomNerdTutorials.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files.
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
*********************************************************************************/

#define _DEBUG_MODE         // uncomment this line to include debug features
#define HOSTNAME "webtach" // <HOSTNAME>.local *should* work for mDNS browsing
#define TCP_PORT           80 // TCP port for ESPAsyncWebServer (80)
#define OLED_I2C         0x3C // display I2C address
#define TACH1_PIN          17
#define TACH2_PIN          18
#define DEFAULT_POLES       4

#include <Wire.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <SSD1306AsciiWire.h>

#include "auth.h"        // edit auth.h with your own wifi and OpenWeatherMap info
#include "tach.h"
#include "web_server.h"

SSD1306AsciiWire oled;
Web_Server server(TCP_PORT);

boolean connectWiFi(int timeout) {
  unsigned long startMillis = millis();
  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(ssid, password);
  WiFi.setHostname(HOSTNAME);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    oled.print(F("."));
    if (millis() - startMillis > timeout) {
      return false;
    }
  }
  return true;
}

void setup(){
  Serial.begin(115200);

  Serial.println(F("\n\n*****    Configuring Web Tachometer    *****\n"));

  tach1.pin = TACH1_PIN;
  tach2.pin = TACH2_PIN;
  tach1.num_poles = DEFAULT_POLES;
  tach2.num_poles = DEFAULT_POLES;

  pinMode(tach1.pin, INPUT_PULLUP);
  attachInterrupt(tach1.pin, trigger1, FALLING);
  pinMode(tach2.pin, INPUT_PULLUP);
  attachInterrupt(tach2.pin, trigger2, FALLING);

  Serial.print(F("  Initializing display..."));
  Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, OLED_I2C);
  Serial.println(F("DONE\n"));

  oled.setFont(lcd5x7);
  oled.clear();
  oled.print(F("Initializing..."));
  
  Serial.print(F("  Preparing SPIFFS..."));
  if(!SPIFFS.begin()){
    Serial.println(F("ERROR: SPIFFS not mounted"));
    return;
  }
  Serial.println(F("READY\n"));

  Serial.printf("  Connecting to %s..", ssid);

  int wifiAttempts = 0;
  while (!connectWiFi(5000)) {
    wifiAttempts++;
    if (wifiAttempts < 3) {
      Serial.printf("RESET %d.", wifiAttempts);
    } else {
      Serial.println(F("FAILED\n\nRebooting!"));
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println(F("CONNECTED\n"));

  Serial.print(F("  Starting mDNS service..."));
  if (!MDNS.begin(HOSTNAME)) {
    Serial.println(F("FAILED\n"));
  }
  Serial.println(F("STARTED\n"));

  Serial.print(F("  Setting time from NTP..."));
  configTime(0, 0, "pool.ntp.org");              // set clock to UTC
  Serial.println(F("SET\n"));

  Serial.println(F("  Starting data services:"));
  server.begin();
  MDNS.addService("http", "tcp", TCP_PORT);

  Serial.printf("\n  Access via: %s / %s.local\n", WiFi.localIP().toString().c_str(), WiFi.getHostname());
  oled.clear();
  oled.printf("IP  : %s\nHost: %s", WiFi.localIP().toString().c_str(), WiFi.getHostname());

  delay(100);
  
  Serial.println(F("\n*****        Configuration Complete!       *****\n"));
}

void loop(){
  if (micros() - tach1.last_triggered > 2000000) {
    tach1.rpm = 0;
  }
  if (micros() - tach2.last_triggered > 2000000) {
    tach2.rpm = 0;
  }
  Serial.print("Tach 1: ");
  Serial.print(tach1.rpm);
  Serial.print("    Tach 2: ");
  Serial.println(tach2.rpm);
  oled.setFont(Callibri11_bold);
  oled.setCursor(0, 3);
  oled.print("RPM 1: ");
  oled.setCursor(0, 6);
  oled.print("RPM 2: ");
  char buf[7];
  dtostrf(tach1.rpm, 5, 0, buf);
  std::replace(buf, buf + 7, ' ', '-');
  oled.setFont(lcdnums14x24);
  oled.setCursor(45, 2);
  oled.print(buf);
  dtostrf(tach2.rpm, 5, 0, buf);
  std::replace(buf, buf + 7, ' ', '-');
  oled.setCursor(45, 5);
  oled.print(buf);
  delay(250);
}
