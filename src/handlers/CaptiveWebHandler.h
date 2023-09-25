#pragma once

#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)

#include "ESPAsyncWebServer.h"

//
// An AsyncWebHandler that redirects any non-local traffic to itself,
// triggering the captive portal
//

class CaptiveWebHandler : public AsyncWebHandler
{
private:
  String hostname;

public:
  CaptiveWebHandler() {}
  virtual ~CaptiveWebHandler() {}

  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);

  String getHostname() { return hostname; }
  void setHostname(String value) { hostname = value; }
};


#endif
