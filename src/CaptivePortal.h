#pragma once

#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
// ...
#endif

#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

#include "Debug.h"
#include "handlers/CaptiveWebHandler.h"

class CaptivePortal
{
private:
  String ssid;
  String passphrase;

  DNSServer dnsServer;
  CaptiveWebHandler handler;

public:
  CaptivePortal(String ssid = "", String passphrase = "") : ssid(ssid), passphrase(passphrase) {}
  virtual ~CaptivePortal() {}

  void begin();
  void loop();
  void end();

  AsyncWebHandler* getHandler() { return &handler; }
};

#endif
