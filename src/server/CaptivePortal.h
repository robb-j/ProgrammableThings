#pragma once

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
// TODO: What are these imports?
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
  /*
    A module for creating a captive portal using the [WiFi interface](https://www.arduino.cc/reference/en/libraries/wifi/) and creating a `DNSServer`.

    ```cpp
    #include <ProgrammableThings.h>
    #include <ESPAsyncWebServer.h>

    AsyncWebServer server(80);
    CaptivePortal portal("My WiFi", "top_secret");

    server.addHandler(portal.getHandler());
    // Add some middleware to serve the HTML page ...
    
    portal.begin();
    server.begin();
    ```
  */
  CaptivePortal(String ssid = "", String passphrase = "") : ssid(ssid), passphrase(passphrase) {}
  virtual ~CaptivePortal() {}

  /*
    Start the captive portal by turning on WiFi in access-point mode
    and configuring the DNS server to always resolve to the local IP address
  */
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

  /* Tick the portal, this needs to be called regularly to ensure DNS requests are handled */
  void loop()
  {
    dnsServer.processNextRequest();
  }

  /* Stop the portal by disconnecting the WiFi and stopping the DNS Server*/
  void end()
  {
    dnsServer.stop();
    WiFi.disconnect(true);
  }

  /* Retrieve a RequestHandler thats pre-configured to do HTTP captive Portal logic */
  AsyncWebHandler* getHandler() { return &handler; }
};
