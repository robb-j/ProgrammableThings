#pragma once

#define FS_NO_GLOBALS

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
// ...
#endif

#include <DNSServer.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>

#include "quickjs/quickjs.h"

#include "handlers/CaptiveWebHandler.h"
#include "handlers/LogWebHandler.h"
#include "handlers/FallbackWebHandler.h"
#include "ProgramEngine.h"
#include "Debug.h"

typedef void(EndpointsCallback)(AsyncWebServer *server);
typedef void(RuntimeCallback)(JSRuntime *runtime, JSContext *context);

class ProgrammableWiFiOptions
{
public:
  const char *ssid = nullptr;
  const char *passphrase = nullptr;
};

class ProgrammableThingOptions
{
public:
  fs::FS *fs = nullptr;
  EndpointsCallback *endpoints = nullptr;
  ProgramEngineOptions *engine = nullptr;
  ProgrammableWiFiOptions *wifi = nullptr;
};

class ProgrammableThing : public AsyncWebHandler
{
private:
  ProgrammableThingOptions *options;

  DNSServer dnsServer;
  AsyncWebServer server = AsyncWebServer(80);
  ProgramEngine engine;

public:
  ProgrammableThing(ProgrammableThingOptions *options) : options(options), engine(options->engine) {}
  virtual ~ProgrammableThing() {}

  void begin();
  void loop();
  void end();

  void endpoints(AsyncWebServer *server);
};
