#include "FallbackWebHandler.h"

bool FallbackWebHandler::canHandle(AsyncWebServerRequest *request)
{
  return true;
}

void FallbackWebHandler::handleRequest(AsyncWebServerRequest *request)
{
  Serial.println("-> fallback route");
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->setCode(200);
  response->print(
      "<!DOCTYPE html><html><head>"
      "<meta charset=\"UTF-8\" />"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
      "</head><body>"
      "<h1>This is a captive portal</h1>"
      "<p>This is not the page you are looking for</p>"
      "</body></html>");
  request->send(response);
}
