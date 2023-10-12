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
  /*
    ProgramEngine encapsulates a JavaScript runtime, powered by [QuickJS](https://bellard.org/quickjs/), to run scripts on microcontrollers from some sort of file system, e.g. SD or SPIFFS. You give a filesystem and directory within it to work with, how much memory to allocate for the JavaScript world and a pointer to a function to customise newly created JavaScript contexts.

    ```cpp
    #include <ProgrammableThings.h>

    void setupJavaScript(JSRuntime *rt, JSContext *ctx, JSValue global)
    {
      auto thing = JS_NewObject(ctx);
      JS_SetPropertyStr(ctx, global, "Thing", thing);
    }

    auto engine = ProgramEngine(&SD, "/scripts/", ESP.getFreeHeap() >> 1, setupJavaScript);
    ```

    This creates an engine that will read and write scripts from the attached [Arduino SD card](https://www.arduino.cc/reference/en/libraries/sd/) within the `scripts` directory. It then allocates half of the available memory for JavaScript to use and passed a pointer to a method that can configure the JavaScript world.

    The setup method is the interesting bit, it provides a hook to let you add custom stuff in the scripts. Here it adds a new object "thing" that the JavaScript can access.

    For more information see [JavaScript](https://github.com/robb-j/ProgrammableThings#javascript).
  */
  ProgramEngine(fs::FS *fs, String dir, uint32_t memoryLimit, EngineCallback *setup) : fs(fs), dir(dir), memoryLimit(memoryLimit), setupCallback(setup) {}
  virtual ~ProgramEngine() {}

  /* Setup the engine so it is ready to run scripts */
  void begin();

  /* This needs to be called regularly to keep the engine going, it does things like JavaScript timers */
  void loop();

  /* Stop the engine and tidy it up after itself */
  void end();

  /* Get the currently running program, or `nullptr` if there isn't one. */
  Program *getProgram() { return program; }

  /* Create a new context for executing JavaScript into with any custom things from the `setup` function injected. */
  JSContext *createContext();

  /* Open and read the contents of a script with the given `filename` from the engine's FS & directory */
  String readScript(String filename);

  /* (over)write the contents of a script with the given `filename` and `source` from the engine's FS & directory */
  void writeScript(String filename, String source);

  /* Check if a given script exists in the engine's FS & directory */
  bool scriptExists(String filename);

  /* Run the given script in the engine */
  Program *runScript(String filename);

  /* Evaluate specific code in the engine and run it as a Program */
  Program *runProgram(String code, String filename = "<eval>");

  /* Stop the current program if there is one */
  void stopProgram();

  /* Get the JavaScript runtime for some manual manipulation */
  JSRuntime *getRuntime() { return rt; }

  /*
    Set the "opaque" value, this can be a pointer to anything thats useful to retrive later. The reference is set on the Program too so you can retrieve your custom object at any point.

    For example, if your app had some NeoPixels you could store the pointer to them here, then in your JavaScript API you can dereference the NeoPixels to interact with them.

    Our best practise is to have a "context" object, like `AppContext` which is just an object with pointers to all the useful things your app is using. Then you can use this as your "opaque" value and have access to everything you need from your JavaScript handlers.

    For more information see [JavaScript](https://github.com/robb-j/ProgrammableThings#javascript).
  */
  void setOpaque(void *ptr) { opaque = ptr; }

  /*
    Retrieve your "opaque" object, set from `setOpaque`. ProgramEngine#setOpaque

    For more information see [JavaScript](https://github.com/robb-j/ProgrammableThings#javascript)
  */
  void *getOpaque() { return opaque; }

  /*
    @internal
    A method for QuickJS to call so we can stop scripts running too long.
  */
  int handleInterrupt(JSRuntime *rt);

  /**
    @internal
    A method for QuickJS format a module name for internal referencing.
  */
  char *normaliseModuleName(JSContext *ctx, String baseName, String moduleName);

  /**
    @internal
    A method for QuickJS use to load a module from the engine's FS and directory
  */
  JSModuleDef *loadModule(JSContext *ctx, String moduleName);
};
