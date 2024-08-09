#ifndef WIREBOUNDWORLDCREATOR_SRC_INTERFACE_H_
#define WIREBOUNDWORLDCREATOR_SRC_INTERFACE_H_

#include "glad/glad.h"

#include "../common/Texture.h"
#include "../common/Shader.h"

#include "Menu.h"
#include "../modes/AllModes.h"
#include "../vbos/AllVbos.h"
#include "TileRenderer.h"
#include "../common/Paths.h"
#include "../io/Window.h"
#include "../io/Camera.h"
#include "../io/Cursor.h"
#include "../common/GlobalGlfwCallbackData.h"

class Interface {
 public:
  Interface(const Paths& paths, Camera& camera, Cursor& cursor);

  void Render();

 private:
  void Init();

  /// shared across all edit modes and menu
  SharedResources shared_resources_;

  // never use directly!
  // used for initializing and render-MODEs (and switching tiles at mode_Tiles)
  //TODO: holds cur_tile_ which is changing only at mode_Tiles
  TileRenderer tile_renderer_;

  Menu menu_;

  IEditMode* cur_mode_{nullptr};
  TerrainMode terrain_; // 1 heightmap
  WaterMode water_; // 1 heightmap + graph-s
  RoadsMode roads_; // graph-s
  FencesMode fences_; // graph-s
  BiomesMode biomes_; // 1 "point" set
  ObjectsMode objects_; // 1 "point" set
  PlacementMode placement_; // 5 textures
  TilesMode tiles_;

  GlobalGlfwCallbackData global_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_INTERFACE_H_
