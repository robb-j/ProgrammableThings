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
  String source;
  String filename;
  uint32_t executionStart = 0;
  void *opaque;

  bool setupModuleMeta(JSContext *ctx, JSModuleDef *mod, String url, bool main);

public:
  /*
    Program is the running of JavaScript source code which could also be told about imports later.

    ```cpp
    auto program = Program(runtime, context, "console.log('hello, there')", "<eval>")
    ```

    It contains logic for setting up the JavaScript context to inject runtime stuff like standard ES timers and also tracks the time spent executing the program, so it can be interupted if neccessary.
  */
  Program(JSRuntime *rt, JSContext *ctx, String source, String filename);
  virtual ~Program() {}

  /* Set up the JavaScript context with custom stuff and start the program going */
  void begin();

  /* Update the program to progress the runtime, e.g. by ticking timers */
  void loop();

  /* Stop the program */
  void end();

  /* Get the time the latest bit of execution started, either the initial run or hooks from the runtime like timers. */
  uint32_t getExecutionStart() { return executionStart; }

  /* Whether the program has run it's course and there is no reason to keep it alive. */
  bool isInactive() { return scheduler.isInactive(); }

  /*
    @internal
    Get the Program's scheduler, the object responsible for timeouts, timers and animation frames
  */
  ProgramScheduler *getScheduler() { return &scheduler; }

  /*
    Set the program's "opaque" value to something to be retrieved later. Similar to ProgramEngine#setOpaque, this lets you keep a referece to something you own on the Program. If created by `ProgramEngine` the Program's opaque value will be set by the engine to whatever it's opaque value is.

    This is useful in your JavaScript bindings so the code that is called from the JavaScript land can dereference this value and interact with the firmware in some way.

    For more information see [JavaScript](https://github.com/robb-j/ProgrammableThings#javascript).
  */
  void setOpaque(void *ptr) { opaque = ptr; }

  /*
    Get the program's "opaque" value and cast it back to it's original type. More info at `Program#setOpaque`

    For more information see [JavaScript](https://github.com/robb-j/ProgrammableThings#javascript).
  */
  template <class T>
  T *getOpaque() { return static_cast<T *>(opaque); }

  /*
    Add an import to the Program and evaluate it's contents, this is usually called by `ProgramEngine` when the QuickJS import callback is triggered. The engine tells the Program of a new import and it evaluates it, adding it into it's context.

    `import.meta.url` in the JavaScript will be set to `moduleName`
  */
  JSModuleDef *addImport(String moduleName, String source);
};
