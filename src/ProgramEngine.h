#pragma once

#include <FS.h>
#include "quickjs/quickjs.h"

#include "Debug.h"
#include "Program.h"

#define JS_INTERUPT_THRESHOLD_MS 500

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx, JSValue global);

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
  uint32_t memoryLimit;
  EngineCallback *setupCallback;

  Program *program = nullptr;
  void *opaque = nullptr;

public:
  ProgramEngine(fs::FS *fs, const char *dir, uint32_t memoryLimit, EngineCallback *setup) : fs(fs), dir(dir), memoryLimit(memoryLimit), setupCallback(setup) {}
  virtual ~ProgramEngine() {}

  // ProgramEngine(ProgramEngineOptions *options) : options(options) {}
  // virtual ~ProgramEngine() {}

  void begin();
  void loop();
  void end();

  Program *getProgram() { return program; }

  JSContext *createContext();

  Program *runScript(String filename);
  Program *runScript(const char *filename) { return runScript(String(filename)); }

  String loadFile(String filename);
  const char *loadFile(const char *filename) { return loadFile(String(filename)).c_str(); }

  Program *runProgram(String code, String filename = "<eval>");
  Program *runProgram(const char *code, const char *filename = "<eval>") {
    return runProgram(String(code), String(filename));
  }
  void stopProgram();

  JSRuntime *getRuntime() { return rt; }

  void setOpaque(void *ptr) { opaque = ptr; }
  void *getOpaque() { return opaque; }

  int handleInterrupt(JSRuntime *rt);
  char *normaliseModuleName(JSContext *ctx, const char *baseName, const char *moduleName);
  JSModuleDef *loadModule(JSContext *ctx, const char *moduleName);
};
