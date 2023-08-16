#include "ProgramEngine.h"

void ProgramEngine::begin()
{
  Serial.println("ProgramEngine::begin");
  Serial.println("- create runtime");
  rt = JS_NewRuntime();

  Serial.println("- create context");
  ctx = JS_NewContext(rt);

  // TODO: is this right?
  Serial.println("- setup memory");
  auto memoryLimit = options->memoryLimit;
  JS_SetMemoryLimit(rt, memoryLimit);
  JS_SetGCThreshold(rt, memoryLimit >> 3);

  // more setup ...
  // https://github.com/binzume/esp32quickjs/blob/ec49bcf3768c3168cce7234c6d2b74d866f3a4a7/esp32/QuickJS.h#L299

  Serial.println("- set context opaque");
  JS_SetContextOpaque(ctx, this);

  JSValue global = JS_GetGlobalObject(ctx);

  // required:
  // - console.log
  // - timers
  // - fetch?!
  // - other web APIs ???

  // Setup console
  Serial.println("- allow config");
  JSValue console = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, global, "console", console);
  JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));

  if (options->setup != nullptr)
  {
    Serial.println("- custom setup");
    options->setup(rt, ctx);
  }

  Serial.println("- done");

  // ... ???
}

void ProgramEngine::loop()
{
  // Tick timers and such?
}

void ProgramEngine::end()
{
  // clean timers
  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);
}

bool ProgramEngine::exec(const char *code)
{
  JSValue result = eval(code);
  bool ex = JS_IsException(result);
  JS_FreeValue(ctx, result);
  return ex;
}

// Callers need to JS_FreeValue(ret)
JSValue ProgramEngine::eval(const char *code)
{
  JSValue result = JS_Eval(ctx, code, strlen(code), "<eval>", JS_EVAL_TYPE_MODULE);
  if (JS_IsException(result))
  {
    dumpException(ctx, result);
  }
  return result;
}

bool ProgramEngine::dumpException(JSContext *ctx, JSValue v)
{
  if (!JS_IsUndefined(v))
  {
    const char *str = JS_ToCString(ctx, v);
    if (str)
    {
      Serial.println(str);
      JS_FreeCString(ctx, str);
    }
    else
    {
      Serial.println("[Exception]");
    }
  }
  JSValue e = JS_GetException(ctx);
  const char *str = JS_ToCString(ctx, e);
  if (str)
  {
    Serial.println(str);
    JS_FreeCString(ctx, str);
  }
  if (JS_IsError(ctx, e))
  {
    JSValue s = JS_GetPropertyStr(ctx, e, "stack");
    if (!JS_IsUndefined(s))
    {
      const char *str = JS_ToCString(ctx, s);
      if (str)
      {
        Serial.println(str);
        JS_FreeCString(ctx, str);
      }
    }
    JS_FreeValue(ctx, s);
  }
  JS_FreeValue(ctx, e);
}
