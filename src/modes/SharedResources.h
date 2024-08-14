#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_

#include "glad/glad.h"

#include "../common/Texture.h"
#include "../common/Shader.h"

#include "../common/Paths.h"
#include "../common/GlobalGlfwCallbackData.h"

#include "../core/Tile.h"

class TileRenderer;

class SharedResources {
 public:
  SharedResources(const Paths& paths,
                  Tile& cur_tile,
                  TileRenderer& tile_renderer_,
                  GlobalGlfwCallbackData& global_glfw_callback_data);

  ~SharedResources() {
    DeInit();
  }

  /// intentionally public
//  Shader shader_ui_;
  Texture tex_text_;
  Texture tex_ui_;

  GLuint vao_ui_;
  GLuint vao_text_; // here render description on hovering

  // BiomesMode, ObjectMode, TilesMode also has their own VAOs with
  // instanced position data

  GlobalGlfwCallbackData& global_glfw_callback_data_;

  GLuint vbo_ui_;
  GLuint vbo_text_;
  GLuint vbo_instanced_;

  TileRenderer& tile_renderer_;
  Shader static_sprite_shader_;
  Shader static_sprite_picking_shader_;
  Shader slider_handle_shader_;

  Tile& tile_;

 private:
  void Init();

  void InitVbos();

  void InitVaos();

  void DeInit();

  void DeInitVaos();

  void DeInitVbos();
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_
