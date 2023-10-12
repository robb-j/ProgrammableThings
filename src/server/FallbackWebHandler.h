#pragma once

#include <ESPAsyncWebServer.h>

/*
  A fallback handler to return some html if nothing else happened

  TODO:
  - only handle html-like requests $path, $path/ or $path/index ?
    or just "/"
*/
class FallbackWebHandler : public AsyncWebHandler
{
  bool canHandle(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *request);
};
