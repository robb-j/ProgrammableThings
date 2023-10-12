#pragma once

#include <vector>
#include "quickjs/quickjs.h"
#include "Debug.h"

// This could be more efficiently packed?
// interval:
//   -1 -> setTimeout
//    0 -> requestAnimationFrame
//   >0 -> setInterval
struct TimerEntry
{
  uint32_t id;
  uint32_t nextTick;
  int32_t interval;
  JSValue fn;
  JSValue thisValue;
};

typedef void(TimerExceptionHandler)(JSContext *ctx, JSValue v);

class ProgramScheduler
{
private:
  std::vector<TimerEntry> timers;
  uint32_t idCounter = 0;
  JSContext *ctx;
  TimerExceptionHandler *exceptionHandler;

public:
  /*
    ProgramScheduler is responsible for managing & processing timers from the JavaScript world.

    ```cpp
    #include <ProgrammableThings.h>

    auto scheduler = new ProgramScheduler(context);
    ```

    It takes the context of the Program being run so it can call the relevant timer callbacks.
  */
  ProgramScheduler(JSContext *ctx) : ctx(ctx) {}
  virtual ~ProgramScheduler() {}

  /*
    Give the scheduler a method to call when a JavaScript error occurs while processing a timer callback
  */
  void setExceptionHandler(TimerExceptionHandler *value) { exceptionHandler = value; }

  /* Get the exception handler */
  TimerExceptionHandler *getExceptionHandler() { return exceptionHandler; }


  /*
    Add a new timer to the schedule.

    ```cpp
    // Add a timeout for 5 seconds
    scheduler->add(jsFunctionValue, millis() + 5000, -1, jsThisValue);
    
    // Run an interval every 3 seconds, starting in 3 seconds
    scheduler->add(jsFunctionValue, millis() + 3000, 3000, jsThisValue);
    
    // Request an animation frame
    scheduler->add(jsFunctionValue, millis(), 0, jsThisValue);
    ```

    - `fn` is a JSValue pointing to a JavaScript function to be called.
    - `nextTick` is the time it should be triggered, in whatever the system time is
    - `interval` is how it should repeat (or not) see `TimerEntry` for info
    - `thisValue` is the "this" that will be available to JavaScript while calling `fn`
  */
  uint32_t add(JSValue fn, uint32_t nextTick, int32_t interval, JSValue thisValue);

  /*
    Remove a timer from the schedule using it's numeric id.
  */
  void clear(uint32_t id);

  /*
    Remove all of the timers, useful when a program is ending
  */
  void clearAll();

  /*
    Tick the timers and run any functions if they should have been triggered.
  */
  void tick(uint32_t now);
  
  /*
    Whether there are any active timers or not.
  */
  bool isInactive();
};
