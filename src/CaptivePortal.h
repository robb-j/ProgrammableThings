#pragma once

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

  void begin()
  {
    Debug::log("Starting captive portal...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, passphrase, 1, 0, 8);

    String selfIp = WiFi.softAPIP().toString();
    Debug::log("WiFi: " + selfIp);

    dnsServer.start(53, "*", WiFi.softAPIP());
    Debug::log("DNS: " + selfIp + ":53");

    handler.setHostname(selfIp);
    Debug::log("HTTP: " + selfIp + ":80");
  }

  void loop()
  {
    dnsServer.processNextRequest();
  }

  void end()
  {
    dnsServer.stop();
    WiFi.disconnect(true);
  }

  AsyncWebHandler* getHandler() { return &handler; }
};
