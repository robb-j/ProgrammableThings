#pragma once

#include <Arduino.h>
#include "quickjs/quickjs.h"

typedef void(EngineCallback)(JSRuntime *rt, JSContext *ctx);

class ProgramEngineOptions
{
public:
  uint32_t memoryLimit;
  EngineCallback *setup;
};

class ProgramEngine
{
private:
  ProgramEngineOptions *options;
  JSRuntime *rt = nullptr;
  JSContext *ctx = nullptr;

  static JSValue js_template(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}

  static JSValue js_console_log(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
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
  // static JSValue js_set_timeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}
  // static JSValue js_clear_timeout(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}
  // static JSValue js_set_interval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}
  // static JSValue js_clear_interval(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}
  // static JSValue js_request_animation_frame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}
  // static JSValue js_cancel_animation_frame(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {}

  bool dumpException(JSContext *ctx, JSValue value);

public:
  ProgramEngine(ProgramEngineOptions *options) : options(options) {}
  virtual ~ProgramEngine() {}

  void begin();
  void loop();
  void end();

  bool exec(const char *code);
  JSValue eval(const char *code);
};
