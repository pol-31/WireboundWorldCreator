#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_

#include <glad/glad.h>

#include "../common/GlfwContext.h"
#include "../common/Shader.h"
#include "../common/Texture.h"

class UiSharedResources {
 public:
  UiSharedResources(GlfwContext& global_glfw_callback_data);

  ~UiSharedResources() { DeInit(); }

  Texture tex_ui_;
  Texture tex_ui_mask_;

  GLuint vao_ui_{0};

  GLuint vbo_ui_{0};
  GLuint vbo_ui_transform_{0};

  Shader shader_sp_;
  Shader shader_sp_mask_;
  Shader shader_sp_picking_;
  Shader shader_sp_hmap_;

  GlfwContext& glfw_context_;

  Shader shader_mdl_;
  Shader shader_animated_mdl_;
  Shader shader_mdl_color_;
  Shader shader_mdl_instanced_;
  Shader shader_mdl_picking_;
  Shader shader_animated_mdl_picking_;
  Shader shader_mdl_selected_;
  Shader shader_animated_mdl_selected_;

 private:
  void Init();

  void InitVbos();

  void InitVaos();

  void DeInit();
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UISHAREDRESOURCES_H_
