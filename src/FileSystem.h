#pragma once

#include <Arduino.h>
#include "Debug.h"

class IFileSystem
{
public:
  virtual ~IFileSystem() {}
  virtual bool fileExists(String filename) = 0;
  virtual String resolvePath(String filename) = 0;
  virtual String readTextFile(String filename) = 0;
  virtual void writeTextFile(String filename, String contents) = 0;
};

#if defined(ESP32) || defined(ESP8266)

#include <FS.h>

class ESPFileSystem : public IFileSystem
{
  fs::FS fs;
  String dir;

public:
  ESPFileSystem(fs::FS &fs, String dir) : fs(fs), dir(dir)
  {
    if (!dir.endsWith("/"))
      dir += '/';
  }

  virtual bool fileExists(String filename)
  {
    if (!filename.startsWith("/")) filename = resolvePath(filename);
    if (!filename.startsWith(dir)) return false;
    return fs.exists(resolvePath(filename));
  }
  virtual String resolvePath(String filename)
  {
    return dir + filename;
  }
  virtual String readTextFile(String filename)
  {
    auto file = fs.open(resolvePath(filename), FILE_READ);
    if (!file)
      return "";
    auto data = file.readString();
    file.close();
    return data;
  };
  virtual void writeTextFile(String filename, String contents)
  {
    auto file = fs.open(resolvePath(filename), FILE_WRITE);
    file.write((const uint8_t *)contents.c_str(), contents.length());
    file.close();
  };
};

#endif
