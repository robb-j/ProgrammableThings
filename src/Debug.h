#pragma once

#include <Arduino.h>

// #ifdef DEBUG
// #define debug (value) Serial.println(value)
// #else
// #define debug (value)()
// #endif

// void Debug::log(const char *message);

// template <typename T>
// void Debug::log(const T &value);

class Debug
{
public:
  static void log(const char *message)
  {
    // #if defined(DEBUG)
    Serial.println(message);
    // #endif
  }

  template <typename T>
  static void log(const T &value)
  {
    // #if defined(DEBUG)
    Serial.println(String(value));
    // #endif
  }

  // void log(const char *message)
  // {
  //   Serial.println(message);
  // }

  // void write(const char *message)
  // {
  //   Serial.print(message);
  // }
};
