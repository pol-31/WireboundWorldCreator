#include <iostream>
#include <string>

// TODO: we still need PLACEMENT (add models loading)

#include <stb_image.h>

#include "io/Window.h"
#include "common/Details.h"

#include "common/Paths.h"
#include "core/WireboundWorldCreator.h"

// TODO: we don't need visibility: "tiles_mode"

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

int main(int argc, char* args[]) {
  std::string world_map_path{};
  /*if (argc == 3) {
    world_map_path = args[1];
  } else if (argc != 2) {
    std::cerr
        << "wrong args:"
        << "\n- param_1 for output dir;"
        << "\n- param_2(optional) for AllTiles.txt;" << std::endl;
    return 0;
  }
  std::string out_dir_path = args[0];*/

  Paths paths(details::kPathsPath);
  if (!world_map_path.empty()) {
    paths.world_map = world_map_path;
  }

  stbi_set_flip_vertically_on_load(true);

  SetupWindow();

  WireboundWorldCreator app(paths);
  app.RunRenderLoop();

  glfwTerminate();
  return 0;
}
