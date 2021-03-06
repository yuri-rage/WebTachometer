/*********************************************************************************
 * web_server.h - extends ESPAsyncWebServer for WebTachometer.ino
 * 
 * -- Yuri - Aug 2021
*********************************************************************************/

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include "tach.h"

class Web_Server : public AsyncWebServer{
  public:
    Web_Server(int port);
    void begin();
};

Web_Server::Web_Server(int port):AsyncWebServer(port) {}

void Web_Server::begin() {

  Serial.print(F("    Adding web server routes..."));

  this->onNotFound([](AsyncWebServerRequest *request){
    Serial.print(F("Bad request, sending 404..."));
    request->send(404, "text/plain", F("404...that's a hard no, super chief."));
    Serial.println(F("SENT"));
  });
  this->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print(F("Index page requested..."));
    request->send(SPIFFS, "/index.html");
    Serial.println(F("SENT"));
  });
  this->on("/favicon.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print(F("Favicon requested..."));
    request->send(SPIFFS, "/favicon.svg");
    Serial.println(F("SENT"));
  });
  this->on("/rpm", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print(F("RPM requested..."));
    char buf[32];
    char buf_rpm2[8];
    dtostrf(tach1.rpm, 3, 1, buf);
    dtostrf(tach2.rpm, 3, 1, buf_rpm2);
    strcat(buf, ",");
    strcat(buf, buf_rpm2);
    request->send_P(200, "text/plain", buf);
    Serial.println(buf);
  });
  this->on("/num_poles", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print(F("num_poles requested..."));
    char buf[32];
    char buf_num_poles2[8];
    dtostrf(tach1.num_poles, 1, 0, buf);
    dtostrf(tach2.num_poles, 1, 0, buf_num_poles2);
    strcat(buf, ",");
    strcat(buf, buf_num_poles2);
    request->send_P(200, "text/plain", buf);
    Serial.println(buf);
  });
  this->on("/num_poles", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("num_poles POST received...");
    AsyncWebParameter* p;
    uint8_t val;
    if (request->hasParam("num_poles1", true)) {
      p = request->getParam("num_poles1", true);
      val = atoi(p->value().c_str());
      tach1.num_poles = val;
      Serial.printf("  tach1.numpoles set: %d\n", val);
    }
    if (request->hasParam("num_poles2", true)) {
      p = request->getParam("num_poles2", true);
      val = atoi(p->value().c_str());
      tach2.num_poles = val;
      Serial.printf("  tach2.numpoles set: %d\n", val);
    }
    request->send(200);
  });

#ifdef _DEBUG_MODE
  // this allows requests from locally stored documents on the client device
  //   (i.e., C:\users\yuri\Documents\debug.html, /home/yuri/Documents/test.html, etc)
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
#endif

  Serial.println(F("DONE"));
  Serial.print(F("    Starting web server..."));
  Serial.println(F("READY"));
  AsyncWebServer::begin();
}

#endif // WEB_SERVER_H
