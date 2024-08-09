#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_

#include "glad/glad.h"

#include "../common/Texture.h"
#include "../common/Shader.h"

#include "../common/Paths.h"
#include "../common/GlobalGlfwCallbackData.h"

class TileRenderer;

class SharedResources {
 public:
  SharedResources(const Paths& paths,
                  TileRenderer& tile_renderer_,
                  GlobalGlfwCallbackData& global_glfw_callback_data_);

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

  GLuint vbo_ui_;
  GLuint vbo_text_;
  GLuint vbo_instanced_;

  TileRenderer& tile_renderer;
  GlobalGlfwCallbackData& global_glfw_callback_data;
  Shader static_sprite_shader;
  Shader slider_handle_shader;

 private:
  void Init();

  void InitVbos();

  void InitVaos();

  void DeInit();

  void DeInitVaos();

  void DeInitVbos();
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_SHAREDRESOURCES_H_
