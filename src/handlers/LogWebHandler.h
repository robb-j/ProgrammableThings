#pragma once

#include "ESPAsyncWebServer.h"

class LogWebHandler : public AsyncWebHandler
{
public:
  LogWebHandler() {}
  virtual ~LogWebHandler() {}

  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};
