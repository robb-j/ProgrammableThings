#pragma once

#include <Arduino.h>
#include "quickjs/quickjs.h"
#include "Debug.h"
#include "Program.h"

#define JS_INTERUPT_THRESHOLD_MS 500

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

  Program *getProgram() { return program; }

  JSContext *createContext();

  Program *runProgram(const char *code, const char *filename);
  void stopProgram();
};

int jsInterruptHandler(JSRuntime *rt, void *opaque);
