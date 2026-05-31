#include <stb_image.h>

#include <iostream>
#include <string>
#include <cstdarg>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include "core/Game.h"


// TODO: JoltPhysics-5.5.0/Build/CMakeLists.txt
// # Set repository root <--------------------------------------commented by me
// #set(PHYSICS_REPO_ROOT ..)

static void TraceImpl(const char *inFMT, ...) {
  // Format the message
  va_list list;
  va_start(list, inFMT);
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), inFMT, list);
  va_end(list);

  // Print to the TTY
  std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage,
                             const char *inFile, JPH::uint inLine) {
  // Print to the TTY
  std::cout << inFile << ":" << inLine << ": (" << inExpression << ") "
            << (inMessage != nullptr ? inMessage : "") << std::endl;

  // Breakpoint
  return true;
};

#endif  // JPH_ENABLE_ASSERTS

int main() {
  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;
  JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)
  JPH::Factory::sInstance = new JPH::Factory();
  JPH::RegisterTypes();

  stbi_set_flip_vertically_on_load(true);
  SetupWindow();
  {
    Game app;
    app.Run();
  }
  glfwTerminate();

  JPH::UnregisterTypes();
  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;

  return 0;
}
