#include "ProgramEngine.h"

JSContext *ProgramEngine::createContext()
{
  Debug::log("ProgramEngine#createContext");
  auto ctx = JS_NewContext(rt);

  // TODO: is this right?
  Debug::log("- setup memory");
  auto memoryLimit = options->memoryLimit;
  JS_SetMemoryLimit(rt, memoryLimit);
  JS_SetGCThreshold(rt, memoryLimit >> 3);

  // more setup ...
  // https://github.com/binzume/esp32quickjs/blob/ec49bcf3768c3168cce7234c6d2b74d866f3a4a7/esp32/QuickJS.h#L299

  Debug::log("- set context opaque");
  // JS_SetContextOpaque(ctx, this);

  // JSValue global = JS_GetGlobalObject(ctx);

  // required:
  // - console.log
  // - timers
  // - fetch?!
  // - other web APIs ???

  // Setup console
  Debug::log("- allow config");
  // JSValue console = JS_NewObject(ctx);
  // JS_SetPropertyStr(ctx, global, "console", console);
  // JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));

  if (options->setup != nullptr)
  {
    Debug::log("- custom setup");
    options->setup(rt, ctx);
  }

  Debug::log("- done");

  // ... ???

  return ctx;
}

void ProgramEngine::begin()
{
  Debug::log("ProgramEngine#begin");
  Debug::log("- create runtime");
  rt = JS_NewRuntime();

  Debug::log("- create program");
  program = run("console.log('starting at ' + Date.now()); setTimeout(() => console.log('done at ' + Date.now()), 1000)", "<eval>");

  Debug::log("- run program");
  program->begin();
}

void ProgramEngine::loop()
{
  // Debug::log("ProgramEngine#loop");
  if (program != nullptr)
  {
    // Debug::log("- loop program");
    program->loop();
    if (!program->isActive())
    {
      Debug::log("- program over");
      program->end();
      program = nullptr;
    }
  }
}

void ProgramEngine::end()
{
  Debug::log("ProgramEngine#begin");
  if (program != nullptr)
  {
    Debug::log("- end program");
    program->end();
    program = nullptr;
  }

  JS_FreeRuntime(rt);
}

// bool ProgramEngine::exec(const char *code)
// {
//   JSValue result = eval(code);
//   bool ex = JS_IsException(result);
//   JS_FreeValue(ctx, result);
//   return ex;
// }

// // Callers need to JS_FreeValue(ret)
// JSValue ProgramEngine::eval(const char *code)
// {
//   auto ctx = createContext();
//   JSValue result = JS_Eval(ctx, code, strlen(code), "<eval>", JS_EVAL_TYPE_MODULE);
//   if (JS_IsException(result))
//   {
//     dumpException(ctx, result);
//   }
//   return result;
// }

Program *ProgramEngine::run(const char *code, const char *filename)
{
  auto context = createContext();
  return new Program(rt, context, code, filename);
}

// bool ProgramEngine::dumpException(JSContext *ctx, JSValue v)
// {
//   if (!JS_IsUndefined(v))
//   {
//     const char *str = JS_ToCString(ctx, v);
//     if (str)
//     {
//       Debug::log(str);
//       JS_FreeCString(ctx, str);
//     }
//     else
//     {
//       Debug::log("[Exception]");
//     }
//   }
//   JSValue e = JS_GetException(ctx);
//   const char *str = JS_ToCString(ctx, e);
//   if (str)
//   {
//     Debug::log(str);
//     JS_FreeCString(ctx, str);
//   }
//   if (JS_IsError(ctx, e))
//   {
//     JSValue s = JS_GetPropertyStr(ctx, e, "stack");
//     if (!JS_IsUndefined(s))
//     {
//       const char *str = JS_ToCString(ctx, s);
//       if (str)
//       {
//         Debug::log(str);
//         JS_FreeCString(ctx, str);
//       }
//     }
//     JS_FreeValue(ctx, s);
//   }
//   JS_FreeValue(ctx, e);
// }
