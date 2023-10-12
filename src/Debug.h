#pragma once

#include <Arduino.h>

/*
  A utility to log debug messages when DEBUG=1 is set
*/
class Debug
{
public:
  static void log(const char *message)
  {
    #if defined(DEBUG)
    Serial.println(message);
    #endif
  }

  template <typename T>
  static void log(const T &value)
  {
    #if defined(DEBUG)
    Serial.println(String(value));
    #endif
  }

};
