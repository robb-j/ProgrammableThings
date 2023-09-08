#include "ProgramEngine.h"

JSContext *ProgramEngine::createContext()
{
  Debug::log("ProgramEngine#createContext");
  auto ctx = JS_NewContext(rt);

  Debug::log("- setup memory");
  JS_SetMemoryLimit(rt, memoryLimit);
  JS_SetGCThreshold(rt, memoryLimit >> 3);

  if (setupCallback != nullptr) {
    auto global = JS_GetGlobalObject(ctx);
    setupCallback(rt, ctx, global);
    JS_FreeValue(ctx, global);
  }

  return ctx;
}

void ProgramEngine::begin()
{
  Debug::log("ProgramEngine#begin");
  Debug::log("- create runtime");
  rt = JS_NewRuntime();
  JS_SetInterruptHandler(rt, jsInterruptHandler, this);
}

void ProgramEngine::loop()
{
  // Debug::log("ProgramEngine#loop");
  if (program != nullptr)
  {
    // Debug::log("- loop program");
    program->loop();
    if (program->isInactive())
    {
      this->stopProgram();
    }
  }
}

void ProgramEngine::end()
{
  Debug::log("ProgramEngine#end");
  if (program != nullptr)
  {
    Debug::log("- end program");
    program->end();
    delete program;
    program = nullptr;
  }

  JS_FreeRuntime(rt);
}

Program *ProgramEngine::runScript(const char *filename)
{
  auto path = String(dir) + filename;
  auto file = fs->open(path, FILE_READ);
  
  if (!file) {
    Debug::log(String() + "Failed to open file: " + path);
    return nullptr;
  }

  auto data = file.readString();
  file.close();
  return runProgram(data, path);
}

Program *ProgramEngine::runProgram(const char *code, const char *filename)
{
  Debug::log("ProgramEngine#run filename=" + String(filename));
  if (program != nullptr)
  {
    this->stopProgram();
  }
  auto context = createContext();
  program = new Program(rt, context, code, filename);
  program->begin();
  return program;
}

void ProgramEngine::stopProgram()
{
  Debug::log("ProgramEngine#stopProgram");
  program->end();
  delete program;
  program = nullptr;
}

int jsInterruptHandler(JSRuntime *rt, void *opaque)
{
  auto engine = static_cast<ProgramEngine *>(opaque);
  // Shouldn't be running anyway...
  if (engine->getProgram() == nullptr)
  {
    return 1;
  }

  if (millis() - engine->getProgram()->getExecutionStart() > JS_INTERUPT_THRESHOLD_MS)
  {
    Debug::log("interrupted JavaScript");
    Debug::log(" now=" + String(millis()));
    Debug::log(" start=" + String(engine->getProgram()->getExecutionStart()));
    Debug::log(" threshold=" + String(JS_INTERUPT_THRESHOLD_MS));
    return 1;
  }
  // else
  // {
  //   Debug::log("no interrupt");
  //   Debug::log(" now=" + String(millis()));
  //   Debug::log(" start=" + String(engine->getProgram()->getExecutionStart()));
  //   Debug::log(" threshold=" + String(JS_INTERUPT_THRESHOLD_MS));
  // }

  return 0;
}
