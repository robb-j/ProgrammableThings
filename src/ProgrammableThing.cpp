#include "ProgrammableThing.h"

void ProgrammableThing::begin()
{
  server.addHandler(new LogWebHandler());

  Debug::log("Applying endpoints");
  endpoints(&server);

  if (customEndpoints != nullptr)
  {
    Debug::log("Applying custom endpoints");
    customEndpoints(&server);
  }

  if (captivePortal != nullptr)
  {
    captivePortal->begin(&server);
  }

  if (programEngine != nullptr)
  {
    programEngine->begin();
  }

  server.begin();
  Debug::log("Started");

  // Debug::log("Run test program");
  // engine.runProgram("console.log('Hello there!')", "<eval>");

  // engine.runProgram("console.log('starting at ' + Date.now()); setTimeout(() => console.log('done at ' + Date.now()), 1000)", "<eval>");

  // engine.runProgram("for (let i = 0; i < 10_000; i++) i;", "<eval>");

  // programEngine->runProgram("for (let i = 0; i < 10_000; i++) console.log(i);", "<eval>");
}

void ProgrammableThing::loop()
{
  if (captivePortal != nullptr)
  {
    captivePortal->loop();
  }

  if (programEngine != nullptr)
  {
    programEngine->loop();
  }
}

void ProgrammableThing::end()
{
  if (captivePortal != nullptr)
  {
    captivePortal->end();
  }

  if (programEngine != nullptr)
  {
    programEngine->end();
  }

  server.end();
}

void ProgrammableThing::endpoints(AsyncWebServer *server)
{
  server->on("/programs/", HTTP_GET, [](AsyncWebServerRequest *request)
             { return request->send(200, "application/json", String() + "{\"msg\":\"ok\"}"); });
}
