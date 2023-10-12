#include "LogWebHandler.h"

bool LogWebHandler::canHandle(AsyncWebServerRequest *request) {
  Serial.println(String(request->methodToString()) + " " + request->host() + request->url());
  return false;
}

void LogWebHandler::handleRequest(AsyncWebServerRequest *request) {}
