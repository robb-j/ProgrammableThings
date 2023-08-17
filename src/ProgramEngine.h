#pragma once

#include <Arduino.h>
#include "quickjs/quickjs.h"
#include "Debug.h"
#include "Program.h"

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx);

class ProgramEngineOptions
{
public:
  uint32_t memoryLimit;
  EngineCallback *setup;
};

class ProgramEngine
{
private:
  ProgramEngineOptions *options;
  JSRuntime *rt = nullptr;

  Program *program = nullptr;

public:
  ProgramEngine(ProgramEngineOptions *options) : options(options) {}
  virtual ~ProgramEngine() {}

  void begin();
  void loop();
  void end();

  // bool exec(const char *code);
  // JSValue eval(const char *code);

  JSContext *createContext();
  Program *run(const char *code, const char *filename);
};
