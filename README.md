# ProgrammableThings

ProgrammableThings is an Arduino library for building IoT devices that can easily be reprogrammed and reconfigured by users with JavaScript. It introduces a new malleable layer allowing dynamic scripting on devices, instead of all functionality that is statically baked into the firmware. The library enables you to quickly create an API between your hardware and the JavaScript scripting layer. We hope this makes IoT more maintainable over time and more resilient to vanishing cloud services.

## Dependencies

The project has no dependencies but it does bundle [QuickJS](https://bellard.org/quickjs/) for the JavaScript engine and a runtime is created on top of that. The HTTP logic is designed to be used with [me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) so you may want to install that, but it is not required.

ProgrammableThings currently supports ESP32 development boards, we're working on ESP8266 devices and we're interested in the RP2040 chip too.

## Install

### PlatformIO

The easiest way to get started is with [PlatformIO](https://platformio.org/) which handles the dependencies best for you. Add a dependency to your `platformio.ini` file:

```ini
[env:xxx]
lib_deps =
  git@github.com:robb-j/ProgrammableThings.git#v0.1.0
```

Or alternatively you can clone or submodule the repository into your `libs` folder.

### Arduino IDE

You can download a zip of the latest from the [GitHub releases](https://github.com/robb-j/ProgrammableThings/releases) and use Arduino IDE's **library manager** feature to install it globally.

## Usage

When you want to use ProgrammableThings, you can import any of the header files for specific features you want or there is a catch-all import `ProgrammableThings.h` which includes everything.

```cpp
#include <Arduino.h>
#include <ProgrammableThings.h>
#include <ESPAsyncWebServer.h>

#include <SPIFFS.h>

auto server = AsyncWebServer(80);
auto portal = CaptivePortal("ProgThing Portal");
auto engine = ProgramEngine(&SPIFFS, "/scripts/", ESP.getFreeHeap() >> 1, JavaScript::setup);
```

For example usage see the [examples](/examples/) directory.

## API

To best see whats going on, see the relevant header files in the source code itself.

### JavaScript

This section describes using the JavaScript engine and how to interact with it beyond what is documented in the header files.

TODO: write more docs on how to do JavaScript stuff

#### setupCallback

The setup method you pass to `ProgramEngine` lets your customise the JavaScript world the scripts will run in. This means you can inject variables, methods and objects that the JavaScript can use to interact with the hardware they are being run on.

**Private by default** — the engine is designed to reveal nothing about the hardware by default, unlike other microcontroller-JavaScript bindings. You might not necessarily trust the scripts that are being run and you don't want your device turned into a spying device. This is especially relevant because a key part of ProgrammableThings is that the scripts on the device are malleable and can change, rather than being baked in when flashing the controller. This means that any interaction with the hardware needs to be specifically designed and programmed to get it working. While this takes more work, it means you can create a cleaner API between the hardware, firmware and scripts.

The setup method is where you create an API between your hardware and the scripts that run on it and you can design that however you like. While `ProgramEngine` is starting a `Program` it will call your `setup` method to customise the JavaScript world which you use to inject your API into it.

Currently, you need to use QuickJS itself to create your API, but we're thinking about easier ways to do this in the future. For example code generation based on documentation-comments.

```cpp
#include <Arduino.h>
#include <ProgrammableThings.h>

class JavaScript
{
public:
  static void setup(JSRuntime *rt, JSContext *ctx, JSValue global)
  {
    // To inject things, create them and set them onto the "global" object
    auto thing = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, global, "Thing", thing);
    JS_SetPropertyStr(ctx, thing, "sayHello", JS_NewCFunction(ctx, JavaScript::sayHello, "sayHello", 1));
  }

  static void sayHello(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    // JS_NewCFunction guarantees that the number of arguments are what you pass to it
    // But there can be extra varadic arguments too if you need, but they may not be in the array
    if (!JS_IsString(argv[0]))
    {
      return JS_ThrowTypeError(ctx, "ERR_INVALID_ARG_TYPE");
    }
    auto name = JS_ToCString(ctx, argv[0]);

    Serial.printLn(String() + "Hello " + name);
  }
};
```

In the JavaScript world you can now call `Thing.sayHello('geoff')` and it will directly call the `sayHello` C method.

#### JavaScript Runtime

Not all of the things that exist in JavaScript are in the runtime, here is a list of things you can use. QuickJS, which powers JavaScript, supports up to the ES2020 version of JavaScript. Additionally, ProgrammableThings adds:

- `console.log`
- `setTimeout`
- `clearTimeout`
- `setInterval`
- `clearInterval`
- `requestAnimationFrame`
- `cancelAnimationFrame`

Another nuance is that Date.now() returns whatever time is on the system which probably isn't the number of milliseconds since the epoch.

#### Opaque engine pointers

You will want the JavaScript world to talk to you hardware in some way and these pointers are the way to do that. When you create a ProgramEngine you can call `ProgramEngine#setOpaque` on it and the engine will store your reference for you until you tell it to stop. You can retrieve this pointer again with `ProgramEngine#getOpaque` and cast it back to your value to access and call methods on it.

This pointer is passed on to any Program that is created too so you can access it on the Program via `Program#getOpaque`. The pointer is then in-turn set on QuickJS's `JSContext` object for you to access in js-c method bindings.

```cpp
#include <ProgrammableThings.h>
#include "AppContext.h"

class JavaScript
{
public:
  static void setup(JSRuntime *rt, JSContext *ctx, JSValue global)
  {
    auto thing = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, global, "Thing", thing);
    JS_SetPropertyStr(ctx, thing, "turnOnLed", JS_NewCFunction(ctx, JavaScript::turnOnLed, "turnOnLed", 5));
  }

  static void turnOnLed(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv)
  {
    getAppContext(ctx)->pixels.setPixelColor(led, 0, 255, 0);
  }

private:
  // A helper method like this is quite useful when you have lots of methods needing to access your Opaque Pointer
  static AppContext *getAppContext(JSContext *ctx)
  {
    return static_cast<Program *>(JS_GetContextOpaque(ctx))
        ->getOpaque<AppContext>();
  }
};
```

#### Memory management

QuickJS uses reference counting to handle its memory and free things when they are no longer needing. I general if you create something in the C world you will want to free it if you want it to get disposed properly. You do this like this:

```cpp
auto thing = JS_NewObject(ctx);
// do something with `thing`
JS_FreeValue(ctx, thing);
```

### Script storage

At the moment, the scripts themselves are implicitly loaded based on the filesystem and directory you pass when creating a `ProgramEngine`. This means you can put JavaScript into that filesystem and run them on the controller. For instance this works with SPIFFS that you can flash onto the device or an SD that you can take out and put on from a computer.

In the future it would be interesting to explore more dynamic ways of getting scripts onto the controller. One option would be to use USB host mode so that the device shows up as a USB storage media on a computer and you can drag and drop files. Another avenue would be a standardised HTTP RESTful API via the CaptivePortal to manage and run the scripts.

### HTTP

There are more plans for HTTP features in the future, but for now there is just a CaptivePortal and you can use an AsyncWebServer yourself. See [examples/CaptivePortal](/examples/captive-portal/) for info on how to do that or the [AsyncWebServer docs](https://github.com/me-no-dev/ESPAsyncWebServer).

## Useful links

- [QuickJS docs](https://bellard.org/quickjs/quickjs.html)
- [QuickJS repository](https://github.com/bellard/quickjs)
- [ArduinoJSON](https://arduinojson.org/)
- [ESPAsyncWebServer repository](https://github.com/)

## Development

### Release

1. Ensure everything is in git
2. Update the changelog to document changes
3. Update the version in `library.json` and `library.properties`
4. Commit the change as `vX.Y.Z`
5. Tag the commit as `vX.Y.Z`
6. Push the commit and tag to GitHub
7. Create a GitHub release from the tag with the contents of the changelog
