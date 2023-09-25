#pragma once

#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)

#include "ESPAsyncWebServer.h"

class LogWebHandler : public AsyncWebHandler
{
public:
  LogWebHandler() {}
  virtual ~LogWebHandler() {}

  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};

#endif
