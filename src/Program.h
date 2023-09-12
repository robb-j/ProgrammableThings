#pragma once

#include <Arduino.h>
#include "quickjs/quickjs.h"

#include "ProgramScheduler.h"
#include "Debug.h"

class Program
{
private:
  JSRuntime *rt = nullptr;
  JSContext *ctx = nullptr;
  ProgramScheduler scheduler;
  const char *source;
  const char *filename;
  uint32_t executionStart = 0;
  void *opaque;

public:
  Program(JSRuntime *rt, JSContext *ctx, const char *source, const char *filename);
  virtual ~Program() {}

  void begin();
  void loop();
  void end();

  uint32_t getExecutionStart() { return executionStart; }
  bool isInactive() { return scheduler.isInactive(); }
  ProgramScheduler *getScheduler() { return &scheduler; }

  template <class T>
  T *getOpaque() { return static_cast<T *>(opaque); }

  void setOpaque(void *ptr) { opaque = ptr; }

  JSModuleDef *addImport(const char *moduleName, const char *source, int length);
};
