#pragma once

#include "quickjs/quickjs.h"

#include "Debug.h"
#include "Program.h"
#include "FileSystem.h"

#define JS_INTERUPT_THRESHOLD_MS 500

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx, JSValue global);

class ProgramEngine
{
private:
  JSRuntime *rt = nullptr;

  IFileSystem *fs;
  uint32_t memoryLimit;
  EngineCallback *setupCallback;

  Program *program = nullptr;
  void *opaque = nullptr;

public:
  ProgramEngine(IFileSystem *fs, uint32_t memoryLimit, EngineCallback *setup) : fs(fs), memoryLimit(memoryLimit), setupCallback(setup) {}
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

  void setOpaque(void *ptr) { opaque = ptr; }
  void *getOpaque() { return opaque; }

  int handleInterrupt(JSRuntime *rt);
  char *normaliseModuleName(JSContext *ctx, String baseName, String moduleName);
  JSModuleDef *loadModule(JSContext *ctx, String moduleName);
};
