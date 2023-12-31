#pragma once

#include "ESPAsyncWebServer.h"

/*
  An `AsyncWebHandler` that logs requests to Serial output
*/
class LogWebHandler : public AsyncWebHandler
{
public:
  LogWebHandler() {}
  virtual ~LogWebHandler() {}

  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};
