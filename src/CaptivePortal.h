#pragma once

#include <FS.h>
#include "Debug.h"

// class CaptivePortalOptions
// {
// public:
//   fs::FS *fs = nullptr;
//   const char *dir = "/";
//   const char *ssid = nullptr;
//   const char *passphrase = nullptr;
// };

class CaptivePortal
{
private:
  fs::FS *fs;
  const char *dir;
  const char *ssid;
  const char *passphrase;
  // CaptivePortalOptions *options;

  DNSServer dnsServer;

public:
  CaptivePortal(fs::FS *fs, const char *dir, const char *ssid = nullptr, const char *passphrase = nullptr) : fs(fs), dir(dir), ssid(ssid), passphrase(passphrase) {}
  virtual ~CaptivePortal() {}

  // CaptivePortal(CaptivePortalOptions *options) : options(options) {}

  void begin(AsyncWebServer *server)
  {
    Debug::log("Starting captive portal...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, passphrase, 1, 0, 8);

    String selfIp = WiFi.softAPIP().toString();
    Debug::log("WiFi: " + selfIp);

    dnsServer.start(53, "*", WiFi.softAPIP());
    Debug::log("DNS: " + selfIp + ":53");

    server->addHandler(new CaptiveWebHandler(selfIp));
    Debug::log("HTTP: " + selfIp + ":80");

    if (fs != nullptr && dir != nullptr)
    {
      Debug::log("Serving FS directory");
      server->serveStatic("/", *fs, "/")
          .setDefaultFile("index.html")
          .setCacheControl("max-age=600");
    }

    server->addHandler(new FallbackWebHandler());
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
};
