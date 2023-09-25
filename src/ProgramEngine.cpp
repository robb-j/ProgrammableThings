#include "ProgramEngine.h"

//
// C methods
//

// return != 0 if the JS code needs to be interrupted
static int jsInterruptHandler(JSRuntime *rt, void *opaque)
{
  return static_cast<ProgramEngine *>(opaque)->handleInterrupt(rt);
}

// return the module specifier (allocated with js_malloc()) or NULL if exception
static char *jsModuleNormaliser(JSContext *ctx, const char *baseName, const char *moduleName, void *opaque)
{
  return static_cast<ProgramEngine *>(opaque)->normaliseModuleName(ctx, baseName, moduleName);
}

static JSModuleDef *jsModuleLoader(JSContext *ctx, const char *moduleName, void *opaque)
{
  return static_cast<ProgramEngine *>(opaque)->loadModule(ctx, moduleName);
}


//
// C++ code
//

JSContext *ProgramEngine::createContext()
{
  Debug::log("ProgramEngine#createContext");
  auto ctx = JS_NewContext(rt);

  Debug::log("- setup memory");
  JS_SetMemoryLimit(rt, memoryLimit);
  JS_SetGCThreshold(rt, memoryLimit >> 3);

  if (setupCallback != nullptr)
  {
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
  JS_SetModuleLoaderFunc(rt, nullptr, jsModuleLoader, this);
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

String ProgramEngine::readScript(String filename)
{
  return fs->readTextFile(filename);
}

void ProgramEngine::writeScript(String filename, String source)
{
  return fs->writeTextFile(filename, source);
}

bool ProgramEngine::scriptExists(String filename)
{
  return fs->fileExists(filename);
}

Program *ProgramEngine::runScript(String filename)
{
  Debug::log("ProgramEngine#runScript filename=" + filename);
  auto data = fs->readTextFile(filename);

  if (data == "")
  {
    return nullptr;
  }

  return runProgram(data, fs->resolvePath(filename));
}

Program *ProgramEngine::runProgram(String code, String filename)
{
  this->stopProgram();
  Debug::log("ProgramEngine#run filename=" + filename);
  auto context = createContext();
  program = new Program(rt, context, code, filename);
  program->setOpaque(opaque);
  program->begin();
  return program;
}

void ProgramEngine::stopProgram()
{
  Debug::log("ProgramEngine#stopProgram");
  if (program == nullptr)
  {
    return;
  }
  program->end();
  delete program;
  program = nullptr;
}

int ProgramEngine::handleInterrupt(JSRuntime *rt)
{
  // Shouldn't be running anyway...
  if (getProgram() == nullptr)
  {
    Debug::log("interrupted JavaScript (no program)");
    return 1;
  }

  if (millis() - getProgram()->getExecutionStart() > JS_INTERUPT_THRESHOLD_MS)
  {
    Debug::log("interrupted JavaScript");
    Debug::log(" now=" + String(millis()));
    Debug::log(" start=" + String(getProgram()->getExecutionStart()));
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

// If needed in the future
char *ProgramEngine::normaliseModuleName(JSContext *ctx, String baseName, String moduleName)
{
  Debug::log("jsModuleNormaliser base=" + baseName + " module=" + moduleName);

  return nullptr;
}

JSModuleDef *ProgramEngine::loadModule(JSContext *ctx, String moduleName)
{
  Debug::log("loadModule module=" + moduleName);

  auto program = static_cast<Program *>(JS_GetContextOpaque(ctx));

  if (!fs->fileExists(moduleName))
  {
    Debug::log("Attempted to read unauthorized module: " + moduleName);
    return nullptr;
  }

  return program->addImport(moduleName, fs->readTextFile(moduleName));
}
