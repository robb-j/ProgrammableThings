#pragma once

#include <FS.h>
#include <Arduino.h>
#include "quickjs/quickjs.h"
#include "Debug.h"
#include "Program.h"

#define JS_INTERUPT_THRESHOLD_MS 500

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx);

// class ProgramEngineOptions
// {
// public:
//   fs::FS *fs = nullptr;
//   const char *dir = "/";
//   uint32_t memoryLimit;
//   EngineCallback *setup;
// };

class ProgramEngine
{
private:
  // ProgramEngineOptions *options;
  JSRuntime *rt = nullptr;

  fs::FS *fs;
  const char *dir;
  const char *mainScript;
  uint32_t memoryLimit;
  // EngineCallback *setupCallback;

  Program *program = nullptr;

public:
  ProgramEngine(fs::FS *fs, const char *dir, const char *mainScript, uint32_t memoryLimit) : fs(fs), dir(dir), mainScript(mainScript), memoryLimit(memoryLimit) {}
  virtual ~ProgramEngine() {}

  // ProgramEngine(ProgramEngineOptions *options) : options(options) {}
  // virtual ~ProgramEngine() {}

  void begin();
  void loop();
  void end();

  Program *getProgram() { return program; }

  JSContext *createContext();

  Program *runScript(String filename) { return runScript(filename.c_str()); }
  Program *runScript(const char *filename);

  Program *runProgram(String code, String filename) { return runProgram(code.c_str(), filename.c_str()); }
  Program *runProgram(const char *code, const char *filename = "<eval>");
  void stopProgram();
};

int jsInterruptHandler(JSRuntime *rt, void *opaque);
