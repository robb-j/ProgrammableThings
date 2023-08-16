#include "ProgrammableThing.h"

void ProgrammableThing::begin()
{
  server.addHandler(new LogWebHandler());

  if (options->wifi != nullptr)
  {
    Serial.println("Starting captive portal...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(options->wifi->ssid, options->wifi->passphrase, 1, 0, 8);

    String selfIp = WiFi.softAPIP().toString();
    Serial.println("WiFi: " + selfIp);

    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("DNS: " + selfIp + "53");

    server.addHandler(new CaptiveWebHandler(selfIp));
    Serial.println("HTTP: " + selfIp + "80");
  }

  if (options->endpoints != nullptr)
  {
    Serial.println("Customising endpoints");
    options->endpoints(&server);
  }

  Serial.println("Starting engine");
  engine.begin();

  if (options->fs != nullptr)
  {
    Serial.println("Serving SPIFFS HTML");
    server.serveStatic("/", *options->fs, "/").setDefaultFile("index.html").setCacheControl("max-age=600");
  }

  server.addHandler(new FallbackWebHandler());
  server.begin();
  Serial.println("Started");

  // TEST...
  auto value = engine.exec("console.log('hello, world!')");
}

void ProgrammableThing::loop()
{
  if (options->wifi != nullptr)
  {
    dnsServer.processNextRequest();
  }

  engine.loop();

  // js engine tick?
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

  // stop js engine?
}
