#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_

#include <glad/glad.h>

#include "../common/Texture.h"
#include "../common/Shader.h"

#include "../common/Paths.h"
#include "../common/GlobalGlfwCallbackData.h"

class UiSharedResources {
 public:
  UiSharedResources(const Paths& paths,
                    GlobalGlfwCallbackData& global_glfw_callback_data);

  ~UiSharedResources() {
    DeInit();
  }

  void UpdateResolution();

  /// intentionally public
  Texture tex_ui_;
  Texture tex_ui_mask_;

  GLuint vao_ui_{0};

  GLuint vbo_ui_{0};
#ifndef NDEBUG
  GLuint vbo_ui_transform_{0};
#endif // NDEBUG

  Shader static_sprite_shader_;
  Shader static_sprite_picking_shader_;
  Shader mask_sprite_shader_;
  Shader dynamic_sprite_shader_;
  Shader dynamic_sprite_picking_shader_;
  Shader static_sprite_progress_shader_;

  Shader hmap_shader_;

  Shader select_square_shader_;

  Shader arbitrary_graph_shader_;

  Shader menu_icon_shader_;

  Shader shader_terrain_selection_;

  GlobalGlfwCallbackData& global_glfw_callback_data_;

  Shader shader_model_;
  Shader shader_model_picking_;
  Shader shader_model_selected_;

 private:
  void Init();

  void InitVbos();

  void InitVaos();

  void DeInit();

  void DeInitVaos();

  void DeInitVbos() const;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_
