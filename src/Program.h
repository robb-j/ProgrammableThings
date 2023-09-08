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

public:
  Program(JSRuntime *rt, JSContext *ctx, const char *source, const char *filename) : rt(rt), ctx(ctx), source(source), filename(filename), scheduler(ctx) {
    scheduler.exceptionHandler = dumpException;
  }
  virtual ~Program() {}

  uint32_t getExecutionStart() { return executionStart; }

  void begin()
  {
    Debug::log("Program#begin");
    JS_SetContextOpaque(ctx, this);

    JSValue global = JS_GetGlobalObject(ctx);

    Debug::log("- setup console");
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, global, "console", console);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));

    Debug::log("- setup timers");
    JS_SetPropertyStr(ctx, global, "setTimeout",
                      JS_NewCFunction(ctx, js_set_timeout, "setTimeout", 2));
    JS_SetPropertyStr(ctx, global, "clearTimeout",
                      JS_NewCFunction(ctx, js_clear_timeout, "clearTimeout", 1));
    JS_SetPropertyStr(ctx, global, "setInterval",
                      JS_NewCFunction(ctx, js_set_interval, "setInterval", 2));
    JS_SetPropertyStr(ctx, global, "clearInterval",
                      JS_NewCFunction(ctx, js_clear_interval, "clearInterval", 1));
    JS_SetPropertyStr(ctx, global, "requestAnimationFrame",
                      JS_NewCFunction(ctx, js_request_animation_frame, "requestAnimationFrame", 1));
    JS_SetPropertyStr(ctx, global, "cancelAnimationFrame",
                      JS_NewCFunction(ctx, js_cancel_animation_frame, "cancelAnimationFrame", 1));

    Debug::log("- date shim");
    JSValue Date = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, global, "Date", Date);
    JS_SetPropertyStr(ctx, Date, "now",
                      JS_NewCFunction(ctx, js_date_now, "now", 0));

    Debug::log("- running script");
    executionStart = millis();
    auto result = JS_Eval(ctx, source, strlen(source), filename, JS_EVAL_TYPE_MODULE);
    Debug::log("- done");

    if (JS_IsException(result))
    {
      Debug::log("- execution failed");
      dumpException(ctx, result);
    }

    JS_FreeValue(ctx, global);
  }
  void loop()
  {
    // Debug::log("Program#tick");
    executionStart = millis();
    scheduler.tick(executionStart);
  }
  void end()
  {
    Debug::log("Program#end");
    scheduler.clearAll();
    JS_FreeContext(ctx);
  }

  bool isInactive()
  {
    return scheduler.isInactive();
  }

private:
  //
  // JS
  //
  static JSValue js_console_log(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    Serial.print("[JS] ");
    for (int i = 0; i < argc; i++)
    {
      const char *str = JS_ToCString(ctx, argv[i]);
      if (str)
      {
        Serial.print(str);
        JS_FreeCString(ctx, str);
      }
      Serial.print(i == argc - 1 ? "\n" : " ");
    }
    return JS_UNDEFINED;
  }

  static JSValue js_set_timeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    if (argc != 2 || !JS_IsFunction(ctx, argv[0]) || !JS_IsNumber(argv[1]))
    {
      return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
    }
    auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
    uint32_t timeout;
    JS_ToUint32(ctx, &timeout, argv[1]);
    auto id = program->scheduler.add(argv[0], millis() + timeout, -1, jsThis);
    return JS_NewUint32(ctx, id);
  }

  static JSValue js_clear_timeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    if (argc != 1 || !JS_IsNumber(argv[0]))
    {
      return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
    }
    auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
    uint32_t id;
    JS_ToUint32(ctx, &id, argv[0]);
    program->scheduler.clear(id);
    return JS_UNDEFINED;
  }

  static JSValue js_set_interval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    if (argc != 2 || !JS_IsFunction(ctx, argv[0]) || !JS_IsNumber(argv[1]))
    {
      return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
    }
    auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
    uint32_t interval;
    JS_ToUint32(ctx, &interval, argv[1]);
    auto id = program->scheduler.add(argv[0], millis() + interval, interval, jsThis);
    return JS_NewUint32(ctx, id);
  }

  static JSValue js_clear_interval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    return js_clear_timeout(ctx, jsThis, argc, argv);
  }
  
  static JSValue js_request_animation_frame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    if (argc != 1 || !JS_IsFunction(ctx, argv[0]))
    {
      return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
    }
    auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
    auto id = program->scheduler.add(argv[0], millis(), 0, jsThis);
    return JS_NewUint32(ctx, id);
  }

  static JSValue js_cancel_animation_frame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    return js_clear_timeout(ctx, jsThis, argc, argv);
  }

  static JSValue js_date_now(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    return JS_NewUint32(ctx, millis());
  }

  static void dumpException(JSContext *ctx, JSValue v)
  {
    if (!JS_IsUndefined(v))
    {
      const char *str = JS_ToCString(ctx, v);
      if (str)
      {
        Debug::log(str);
        JS_FreeCString(ctx, str);
      }
      else
      {
        Debug::log("[Exception]");
      }
    }
    JSValue exception = JS_GetException(ctx);
    const char *str = JS_ToCString(ctx, exception);
    if (str)
    {
      Debug::log(str);
      JS_FreeCString(ctx, str);
    }
    if (JS_IsError(ctx, exception))
    {
      JSValue stack = JS_GetPropertyStr(ctx, exception, "stack");
      if (!JS_IsUndefined(stack))
      {
        const char *str = JS_ToCString(ctx, stack);
        if (str)
        {
          Debug::log(str);
          JS_FreeCString(ctx, str);
        }
      }
      JS_FreeValue(ctx, stack);
    }
    JS_FreeValue(ctx, exception);
  }
};
