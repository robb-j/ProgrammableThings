#include "ProgramEngine.h"

JSContext *ProgramEngine::createContext()
{
  Debug::log("ProgramEngine#createContext");
  auto ctx = JS_NewContext(rt);

  Debug::log("- setup memory");
  auto memoryLimit = options->memoryLimit;
  JS_SetMemoryLimit(rt, memoryLimit);
  JS_SetGCThreshold(rt, memoryLimit >> 3);

  if (options->setup != nullptr)
  {
    Debug::log("- custom setup");
    options->setup(rt, ctx);
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
  //   Debug::log("no interript");
  //   Debug::log(" now=" + String(millis()));
  //   Debug::log(" start=" + String(engine->getProgram()->getExecutionStart()));
  //   Debug::log(" threshold=" + String(JS_INTERUPT_THRESHOLD_MS));
  // }

  return 0;
}
