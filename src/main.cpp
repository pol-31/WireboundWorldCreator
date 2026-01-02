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

int main(int argc, char* args[]) {
  stbi_set_flip_vertically_on_load(true);
  SetupWindow();
  {
    WireboundWorldCreator app;
    app.RunRenderLoop();
  }
  glfwTerminate();
  return 0;
}
