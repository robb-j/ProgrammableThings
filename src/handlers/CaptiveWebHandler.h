#pragma once
#include "ESPAsyncWebServer.h"

//
// An AsyncWebHandler that redirects any non-local traffic to itself,
// triggering the captive portal
//

class CaptiveWebHandler : public AsyncWebHandler
{
private:
  String selfHostname;

public:
  CaptiveWebHandler(String selfHostname) : selfHostname(selfHostname) {}
  virtual ~CaptiveWebHandler() {}

  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};
