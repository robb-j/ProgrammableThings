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

typedef void(TimerExceptionHandler)(JSValue *value);

class ProgramScheduler
{
  std::vector<TimerEntry> timers;
  uint32_t idCounter = 0;
  JSContext *ctx;

public:
  ProgramScheduler(JSContext *ctx) : ctx(ctx) {}
  virtual ~ProgramScheduler() {}

  TimerExceptionHandler *exceptionHandler;

  uint32_t add(JSValue fn, uint32_t nextTick, int32_t interval, JSValue thisValue)
  {
    Debug::log("ProgramScheduler#add");
    auto id = ++idCounter;
    JS_DupValue(ctx, fn);
    JS_DupValue(ctx, thisValue);
    timers.push_back(TimerEntry{id, nextTick, interval, fn, thisValue});
    return id;
  }
  void clear(uint32_t id)
  {
    Debug::log("ProgramScheduler#clear");

    auto item = timers.begin();
    while (item != timers.end())
    {
      if (item->id == id)
      {
        JS_FreeValue(ctx, item->fn);
        JS_FreeValue(ctx, item->thisValue);
        item = timers.erase(item);
      }
      else
      {
        item++;
      }
    }
  }

  void clearAll()
  {
    Debug::log("ProgramScheduler#clearAll");

    auto item = timers.begin();
    while (item != timers.end())
    {
      JS_FreeValue(ctx, item->fn);
      JS_FreeValue(ctx, item->thisValue);
      item = timers.erase(item);
    }
  }

  void tick(uint32_t now)
  {
    // Debug::log("ProgramScheduler#tick");

    auto item = timers.begin();
    while (item != timers.end())
    {
      if (item->nextTick > now)
      {
        // Debug::log("- skip: " + item->id);
        item++;
        continue;
      }

      Debug::log("- running: " + item->id);

      // TODO: set "this" properly?
      // TODO: store & pass in args too?
      // TODO: something with the interupt to prevent while(true)
      auto result = JS_Call(ctx, item->fn, item->thisValue, 0, nullptr);
      if (JS_IsException(result) && exceptionHandler != nullptr)
      {
        exceptionHandler(&result);
      }
      JS_FreeValue(ctx, item->fn);
      JS_FreeValue(ctx, item->thisValue);

      if (item->interval == -1)
      {
        Debug::log("  disposing");
        JS_FreeValue(ctx, item->fn);
        item = timers.erase(item);
      }
      else
      {
        Debug::log("  rescheduling");
        item->nextTick += item->interval;
        item++;
      }
    }
  }

  bool isInactive()
  {
    return timers.empty();
  }
};
