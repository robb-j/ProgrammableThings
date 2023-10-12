#include <Arduino.h>
#include <ProgrammableThings.h>
#include <ESPAsyncWebServer.h>

#include <SPIFFS.h>

auto server = AsyncWebServer(80);
auto portal = CaptivePortal("ProgThing Portal");
auto engine = ProgramEngine(&SPIFFS, "/scripts/", ESP.getFreeHeap() >> 1, JavaScript::setup);

AppContext *app;

void setup()
{
  // Setup serial for log messages and mount the spiffs drive
  Serial.begin(115200);
  SPIFFS.begin();

  // Add a utility to log HTTP requests
  server.addHandler(new LogWebHandler());

  // Add CaptivePortal routes to trigger and handle the portal,
  // redirecting most traffic to the local network
  server.addHandler(portal.getHandler());

  // Set the contents of the SPIFFS directory over HTTP,
  // i.e. static HTML/CSS/JS assets for a website
  server.serveStatic("/", SPIFFS, "/www/")
      .setDefaultFile("index.html")
      .setCacheControl("max-age=600");

  // Add a utility to return an error page if there is nothing in the SPIFFS
  server.addHandler(new FallbackWebHandler());

  // Start the captive portal and HTTP server
  portal.begin();
  server.begin();
}

void loop()
{
  // Tick the portal to process DNS requests
  portal.loop();
}
