#include "ProgramScheduler.h"

uint32_t ProgramScheduler::add(JSValue fn, uint32_t nextTick, int32_t interval, JSValue thisValue)
{
  Debug::log("ProgramScheduler#add");
  auto id = ++idCounter;
  JS_DupValue(ctx, fn);
  JS_DupValue(ctx, thisValue);
  timers.push_back(TimerEntry{id, nextTick, interval, fn, thisValue});
  return id;
}
void ProgramScheduler::clear(uint32_t id)
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

void ProgramScheduler::clearAll()
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

void ProgramScheduler::tick(uint32_t now)
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

    auto result = JS_Call(ctx, item->fn, item->thisValue, 0, nullptr);
    if (JS_IsException(result) && exceptionHandler != nullptr)
    {
      exceptionHandler(ctx, result);
    }

    if (item->interval == -1 || item->interval == 0)
    {
      Debug::log("- disposing");
      JS_FreeValue(ctx, item->fn);
      JS_FreeValue(ctx, item->thisValue);
      item = timers.erase(item);
    }
    else
    {
      // Debug::log("- rescheduling");
      item->nextTick += item->interval;
      item++;
    }
  }
}

bool ProgramScheduler::isInactive()
{
  return timers.empty();
}
