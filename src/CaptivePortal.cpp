#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)
#include "CaptivePortal.h"

void CaptivePortal::begin()
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

void CaptivePortal::loop()
{
  dnsServer.processNextRequest();
}

void CaptivePortal::end()
{
  dnsServer.stop();
  WiFi.disconnect(true);
}

#endif
