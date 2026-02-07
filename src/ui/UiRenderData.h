#ifndef WIREBOUNDWORLDCREATOR_UIRENDERDATA_H
#define WIREBOUNDWORLDCREATOR_UIRENDERDATA_H

#include <glad/glad.h>

#include "../common/GlfwContext.h"
#include "../render/Texture.h"
#include "../render/Shader.h"

class UiRenderData {
public:
  UiRenderData(GlfwContext& global_glfw_callback_data);

  ~UiRenderData() { DeInit(); }

  Texture tex_ui_;
  Texture tex_ui_mask_;

  GLuint vao_ui_ = 0;
  GLuint vbo_ui_ = 0;
  GLuint vbo_ui_transform_ = 0;

  Shader shader_sp_;
  Shader shader_sp_mask_;
  Shader shader_sp_picking_;
  Shader shader_sp_hmap_;

  //TODO: take TextRenderer here, remove other (?)
  GlfwContext& glfw_context_;

private:
  void Init();

  void InitVbos();

  void InitVaos();

  void DeInit();
};

#endif  // WIREBOUNDWORLDCREATOR_UIRENDERDATA_H
