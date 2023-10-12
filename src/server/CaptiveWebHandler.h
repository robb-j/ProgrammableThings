#pragma once

#include <ESPAsyncWebServer.h>

/*
  An `AsyncWebHandler` that redirects any non-local traffic to itself, triggering the captive portal. Used internally by CaptivePortal to create the captive-portal effect.
*/
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
