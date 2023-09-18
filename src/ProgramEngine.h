#pragma once

#include <FS.h>
#include "quickjs/quickjs.h"

#include "Debug.h"
#include "Program.h"

#define JS_INTERUPT_THRESHOLD_MS 500

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx, JSValue global);

class ProgramEngine
{
private:
  JSRuntime *rt = nullptr;

  fs::FS *fs;
  String dir;
  uint32_t memoryLimit;
  EngineCallback *setupCallback;

  Program *program = nullptr;
  void *opaque = nullptr;

  String readFile(String filename);

  void writeFile(String filename, String contents);

public:
  ProgramEngine(fs::FS *fs, String dir, uint32_t memoryLimit, EngineCallback *setup) : fs(fs), dir(dir), memoryLimit(memoryLimit), setupCallback(setup) {}
  virtual ~ProgramEngine() {}

  void begin();
  void loop();
  void end();

  Program *getProgram() { return program; }

  JSContext *createContext();

  String readScript(String filename);
  void writeScript(String filename, String source);
  bool scriptExists(String filename);

  Program *runScript(String filename);
  Program *runProgram(String code, String filename = "<eval>");
  void stopProgram();

  JSRuntime *getRuntime() { return rt; }
  String getDir() { return dir; }

  void setOpaque(void *ptr) { opaque = ptr; }
  void *getOpaque() { return opaque; }

  int handleInterrupt(JSRuntime *rt);
  char *normaliseModuleName(JSContext *ctx, String baseName, String moduleName);
  JSModuleDef *loadModule(JSContext *ctx, String moduleName);
};
