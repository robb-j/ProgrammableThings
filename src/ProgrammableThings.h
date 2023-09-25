#pragma once

#if defined(PT_TARGET_ESP32) || defined(PT_TARGET_ESP8266)
#include "CaptivePortal.h"
#include "handlers/CaptiveWebHandler.h"
#include "handlers/FallbackWebHandler.h"
#include "handlers/LogWebHandler.h"
#endif

#include "Debug.h"
#include "Program.h"
#include "ProgramEngine.h"
#include "ProgramScheduler.h"

#include "quickjs/quickjs.h"
