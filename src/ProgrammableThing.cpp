#include "ProgrammableThing.h"

void ProgrammableThing::begin()
{
  server.addHandler(new LogWebHandler());

  Debug::log("Applying endpoints");
  endpoints(&server);

  if (options->endpoints != nullptr)
  {
    Debug::log("Applying custom endpoints");
    options->endpoints(&server);
  }

  if (options->wifi != nullptr)
  {
    Debug::log("Starting captive portal...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(options->wifi->ssid, options->wifi->passphrase, 1, 0, 8);

    String selfIp = WiFi.softAPIP().toString();
    Debug::log("WiFi: " + selfIp);

    dnsServer.start(53, "*", WiFi.softAPIP());
    Debug::log("DNS: " + selfIp + ":53");

    server.addHandler(new CaptiveWebHandler(selfIp));
    Debug::log("HTTP: " + selfIp + ":80");
  }

  Debug::log("Starting engine");
  engine.begin();

  if (options->fs != nullptr)
  {
    Debug::log("Serving SPIFFS directory");
    server.serveStatic("/", *options->fs, "/").setDefaultFile("index.html").setCacheControl("max-age=600");
  }

  server.addHandler(new FallbackWebHandler());
  server.begin();
  Debug::log("Started");

  Debug::log("Run test program");
  // engine.runProgram("console.log('Hello there!')", "<eval>");

  // engine.runProgram("console.log('starting at ' + Date.now()); setTimeout(() => console.log('done at ' + Date.now()), 1000)", "<eval>");

  // engine.runProgram("for (let i = 0; i < 10_000; i++) i;", "<eval>");

  engine.runProgram("for (let i = 0; i < 10_000; i++) console.log(i);", "<eval>");
}

void ProgrammableThing::loop()
{
  if (options->wifi != nullptr)
  {
    dnsServer.processNextRequest();
  }

  engine.loop();
}

void ProgrammableThing::end()
{
  server.end();
  engine.end();

  if (options->wifi != nullptr)
  {
    dnsServer.stop();
    WiFi.disconnect(true);
  }
}

void ProgrammableThing::endpoints(AsyncWebServer *server)
{
  server->on("/programs/", HTTP_GET, [](AsyncWebServerRequest *request)
             { return request->send(200, "application/json", String() + "{\"msg\":\"ok\"}"); });
}
