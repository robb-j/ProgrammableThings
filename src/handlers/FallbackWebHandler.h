#pragma once

#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)

#include "ESPAsyncWebServer.h"

//
// A fallback handler to return some html if nothing else happened
//
// TODO:
// - only handle html-like requests $path, $path/ or $path/index ?
//   or just "/"
//

class FallbackWebHandler : public AsyncWebHandler
{
  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};

#endif
