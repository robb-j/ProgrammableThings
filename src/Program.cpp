#include "Program.h"

//
// JS Code
//

static JSValue jsConsoleLog(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
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

static JSValue jsSetTimeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  if (argc != 2 || !JS_IsFunction(ctx, argv[0]) || !JS_IsNumber(argv[1]))
  {
    return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
  }
  auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
  uint32_t timeout;
  JS_ToUint32(ctx, &timeout, argv[1]);
  auto id = program->getScheduler()->add(argv[0], millis() + timeout, -1, jsThis);
  return JS_NewUint32(ctx, id);
}

static JSValue jsClearTimeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  if (argc != 1 || !JS_IsNumber(argv[0]))
  {
    return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
  }
  auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
  uint32_t id;
  JS_ToUint32(ctx, &id, argv[0]);
  program->getScheduler()->clear(id);
  return JS_UNDEFINED;
}

static JSValue jsSetInterval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  if (argc != 2 || !JS_IsFunction(ctx, argv[0]) || !JS_IsNumber(argv[1]))
  {
    return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
  }
  auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
  uint32_t interval;
  JS_ToUint32(ctx, &interval, argv[1]);
  auto id = program->getScheduler()->add(argv[0], millis() + interval, interval, jsThis);
  return JS_NewUint32(ctx, id);
}

static JSValue jsClearInterval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  return jsClearTimeout(ctx, jsThis, argc, argv);
}

static JSValue jsRequestAnimationFrame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  if (argc != 1 || !JS_IsFunction(ctx, argv[0]))
  {
    return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
  }
  auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));
  auto id = program->getScheduler()->add(argv[0], millis(), 0, jsThis);
  return JS_NewUint32(ctx, id);
}

static JSValue jsCancelAnimationFrame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  return jsClearTimeout(ctx, jsThis, argc, argv);
}

static JSValue jsDateNow(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
{
  return JS_NewUint32(ctx, millis());
}

static void jsDumpException(JSContext *ctx, JSValue v)
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

//
// C++ code
//

Program::Program(JSRuntime *rt, JSContext *ctx, String source, String filename) : rt(rt), ctx(ctx), source(source), filename(filename), scheduler(ctx)
{
  scheduler.exceptionHandler = jsDumpException;
}

void Program::begin()
{
  Debug::log("Program#begin");
  JS_SetContextOpaque(ctx, this);

  JSValue global = JS_GetGlobalObject(ctx);

  Debug::log("- setup console");
  JSValue console = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, global, "console", console);
  JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, jsConsoleLog, "log", 1));

  Debug::log("- setup timers");
  JS_SetPropertyStr(ctx, global, "setTimeout",
                    JS_NewCFunction(ctx, jsSetTimeout, "setTimeout", 2));
  JS_SetPropertyStr(ctx, global, "clearTimeout",
                    JS_NewCFunction(ctx, jsClearInterval, "clearTimeout", 1));
  JS_SetPropertyStr(ctx, global, "setInterval",
                    JS_NewCFunction(ctx, jsSetInterval, "setInterval", 2));
  JS_SetPropertyStr(ctx, global, "clearInterval",
                    JS_NewCFunction(ctx, jsClearInterval, "clearInterval", 1));
  JS_SetPropertyStr(ctx, global, "requestAnimationFrame",
                    JS_NewCFunction(ctx, jsRequestAnimationFrame, "requestAnimationFrame", 1));
  JS_SetPropertyStr(ctx, global, "cancelAnimationFrame",
                    JS_NewCFunction(ctx, jsCancelAnimationFrame, "cancelAnimationFrame", 1));

  Debug::log("- date shim");
  JSValue Date = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, global, "Date", Date);
  JS_SetPropertyStr(ctx, Date, "now",
                    JS_NewCFunction(ctx, jsDateNow, "now", 0));

  Debug::log("- running script");
  executionStart = millis();
  auto result = JS_Eval(ctx, source.c_str(), source.length(), filename.c_str(), JS_EVAL_TYPE_MODULE);
  Debug::log("- evaluated");

  if (JS_IsException(result))
  {
    Debug::log("- execution failed");
    jsDumpException(ctx, result);
  }

  // Set import meta
  // Debug::log("- setup module metadata");
  // JSModuleDef *mod = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(result));
  // setupModuleMeta(ctx, mod, filename, true);
  // TODO: I think the module needs to be COMPILE_ONLY first, then set meta, then JS_EvalFunction

  JS_FreeValue(ctx, global);
  JS_FreeValue(ctx, result);

  Debug::log("- done");
}

void Program::loop()
{
  // Debug::log("Program#tick");
  executionStart = millis();
  scheduler.tick(executionStart);
}

void Program::end()
{
  Debug::log("Program#end");
  scheduler.clearAll();
  JS_FreeContext(ctx);
}

// https://github.com/bellard/quickjs/blob/2788d71e823b522b178db3b3660ce93689534e6d/quickjs-libc.c#L567
JSModuleDef *Program::addImport(String moduleName, String source)
{
  Debug::log(String() + "import module=" + moduleName);
  auto result = JS_Eval(ctx, source.c_str(), source.length(), moduleName.c_str(), JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
  Debug::log("- imported");

  if (JS_IsException(result))
  {
    jsDumpException(ctx, result);
    return nullptr;
  }

  // Set import meta
  Debug::log("- setup module metadata");
  JSModuleDef *mod = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(result));
  auto meta = setupModuleMeta(ctx, mod, moduleName, false);
  if (!meta) {
    return nullptr;
  }

  Debug::log("- cleanup");
  JS_FreeValue(ctx, meta);
  JS_FreeValue(ctx, result);

  Debug::log("- done");
  return mod;
}

bool Program::setupModuleMeta(JSContext *ctx, JSModuleDef *mod, String url, bool main)
{
  auto meta = JS_GetImportMeta(ctx, mod);
  if (JS_IsException(meta))
  {
    jsDumpException(ctx, meta);
    return false;
  }
  JS_DefinePropertyValueStr(ctx, meta, "url", JS_NewString(ctx, url.c_str()), JS_PROP_C_W_E);
  JS_DefinePropertyValueStr(ctx, meta, "main", JS_NewBool(ctx, 0), JS_PROP_C_W_E);

  return true;
}
