#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)

#include "CaptiveWebHandler.h"

bool CaptiveWebHandler::canHandle(AsyncWebServerRequest *request)
{
  return this->hostname.length() > 0 && request->host() != this->hostname;
}

// https://datatracker.ietf.org/doc/html/rfc6585#section-6.1
void CaptiveWebHandler::handleRequest(AsyncWebServerRequest *request)
{
  if (request->host() == "netcts.cdn-apple.com") {
    Serial.println("-> 503 Service Unavailable");
    return request->send(503, "text/plain", "Service Unavailable");
  }
  
  Serial.println("-> redirect " + request->host() + request->url() +  " -> " + hostname);
  
  request->redirect(String("http://") + this->hostname);
  
  // auto location = "http://" + this->selfHostname;
  // AsyncResponseStream *response = request->beginResponseStream("text/html");
  // response->setCode(511);
  // response->print(
  //   "<!DOCTYPE html><html><head>"
  //   "<meta charset=\"UTF-8\" />"
  //   "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
  //   "<meta http-equiv=\"refresh\" content=\"0; url=" + location + "\">"
  //   "</head><body>"
  //   "<p><a href=\"" + location + "\">"
  //   "Loading …</a></p>"
  //   "</body></html>"
  // );
  // request->send(response);
}

#endif
