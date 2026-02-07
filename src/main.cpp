#include <stb_image.h>

#include <iostream>
#include <string>

#include "core/WireboundWorldCreator.h"
#include "io/Window.h"

// TODO: after window size changing we should recreate all fbos
//  with new resolution

// TODO: rearrange dir/files - now all important files hided,
//  while assisting here with main.cpp

// TODO (for now):
//  - draw ui, implement switching between modes
//  - idk how to bake.. then how to represent water.........................

// TODO: check all OpenGL objects deletion (destructors::DeInit()) !!!

// TODO; Texture water_caustics_;

// TODO: we can replace colors in Placement_mode to another one UiSlider,
//  so don't need other 3 colors (only white and black), but
//  then we want to show currect color, so need special shader / ui_component

// TODO: we can prerender all picking framebuffer (!)

// TODO: remove utility::UnBindImageTexture at NDEBUG

// TODO: check all classes declaration order:
/* damn...
 * In C++, member variables are initialized in the order they are declared
 * in the class, not in the order they appear in the initializer list.
 * */

/// in Ui*Mode everything's public, otherwise need to make too much callback
/// friends
///

// TODO: JoltPhysics-5.5.0/Build/CMakeLists.txt
// # Set repository root <--------------------------------------commented by me
// #set(PHYSICS_REPO_ROOT ..)

/**
 * NAMING CONVENTION
tex texture (GL_TEXTURE_*)
sp sprite (non-clickable picture)
tg toggle (on/off)
sl slider (hor vert, 2, 3, 2d, etc)
mdl model (3d obj)
btn button (clickable picture)
ui_ ui (complex ui component)
 */

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include <cstdarg>

JPH_SUPPRESS_WARNINGS

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

int main(int argc, char *args[]) {
  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;
  JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)
  JPH::Factory::sInstance = new JPH::Factory();
  JPH::RegisterTypes();

  stbi_set_flip_vertically_on_load(true);
  SetupWindow();
  {
    WireboundWorldCreator app;
    app.RunRenderLoop();
  }
  glfwTerminate();

  JPH::UnregisterTypes();
  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;

  return 0;
}
