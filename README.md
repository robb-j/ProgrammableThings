# ProgrammableThings

Reconfigurable stuff on microcontrollers. ProgrammableThings introduces a new re-programmable layer to microcontroller firmware to allow dynamic scripting of devices rather than static firmware.
The idea is to allow different ways of getting JavaScript scripts onto a microcontroller which have controlled access to relevant hardware and peripherals.

This takes the form of a library of modules to enable you to create an API between your hardware and scripting and some other useful tools like creating captive portals.

## Dependencies

The project has no dependencies but it does bundle [QuickJS](https://bellard.org/quickjs/) for the JavaScript engine and a runtime is created on top of that. The HTTP logic is designed to be used with [me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) so you may want to install that to use it but it is not required.

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

- setup method
- the opaque pointers
- memory management
- "JavaScript" class practice
- Parsing things from the JavaScript world
- Preparing things from the C world

### HTTP

TODO: docs on HTTP stuff

## Development

### Release

1. Ensure everything is in git
2. Update the changelog to document changes
3. Update the version in `library.json` and `library.properties`
4. Commit the change as `vX.Y.Z`
5. Tag the commit as `vX.Y.Z`
6. Push the commit and tag to GitHub
7. Create a GitHub release from the tag with the contents of the changelog
