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
#include "CaptivePortal.h"

typedef void(EndpointsCallback)(AsyncWebServer *server);
// typedef void(RuntimeCallback)(JSRuntime *runtime, JSContext *context);

class ProgrammableWiFiOptions
{
public:
  const char *ssid = nullptr;
  const char *passphrase = nullptr;
};

// class ProgrammableThingOptions
// {
// public:
//   fs::FS *fs = nullptr;
//   EndpointsCallback *endpoints = nullptr;
//   ProgramEngineOptions *engine = nullptr;
//   ProgrammableWiFiOptions *wifi = nullptr;
// };

// class CaptivePortalOptions
// {
// public:
//   fs::FS *fs;
//   const char *dir = "/";
//   const char *ssid = nullptr;
//   const char *passphrase = nullptr;

//   CaptivePortalOptions(fs::FS *fs, const char *dir = "/", const char *ssid = nullptr, const char *passphrase = nullptr) : fs(fs), dir(dir), ssid(ssid), passphrase(passphrase){}
// };

// class ProgramEngineOptions
// {
// public:
//   fs::FS *fs = nullptr;
//   const char *dir = "/";
//   uint32_t memoryLimit;

//   ProgramEngineOptions(fs::FS *fs, const char *dir = "/") : fs(fs), dir(dir) {}
// };

class ProgrammableThing : public AsyncWebHandler
{
private:
  // ProgrammableThingOptions *options;

  
  AsyncWebServer server = AsyncWebServer(80);
  ProgramEngine *programEngine = nullptr;
  CaptivePortal *captivePortal = nullptr;
  EndpointsCallback *customEndpoints = nullptr;

  // fs::FS *captivePortalFs = nullptr;
  // const char *captivePortalDir = "/";

  // fs::FS *scriptEngineFs = nullptr;
  // const char *scriptEngineDir = "/";

public:
  ProgrammableThing() {}
  virtual ~ProgrammableThing() {}

  void setProgramEngine(ProgramEngine *engine) { programEngine = engine; }
  void setCaptivePortal(CaptivePortal *portal) { captivePortal = portal; }
  void setEndpoints(EndpointsCallback *endpoints) { customEndpoints = endpoints; }

  void begin();
  void loop();
  void end();

  void endpoints(AsyncWebServer *server);
};
