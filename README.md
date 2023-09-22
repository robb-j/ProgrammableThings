# ProgrammableThings

Reconfigurable stuff on microcontrollers

**dev notes**

- Timers may mutate during tick
- What are jobs and do I need them?
  https://262.ecma-international.org/14.0/#job
- Do I need to free more things, ie global object.
- The scheduler should make sure the next tick > millis()
- These frees should be above "erase" [ref](https://github.com/robb-j/ProgrammableThings/blob/main/src/ProgramScheduler.h#L103)

```
while (item->nextTick < millis())
{
  item->nextTick += item->interval
}
```
