#include "UiRenderData.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common/Vbos.h"
#include "../io/Window.h"

UiRenderData::UiRenderData(GlfwContext& global_glfw_callback_data)
    : tex_ui_("../assets/TexAtlas.png", GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
      tex_ui_mask_("../assets/tex_ui_mask.png", GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
      shader_sp_mask_("../shaders/Sprite.vert", "../shaders/SpriteMask.frag", {0, 6}),
      shader_sp_("../shaders/Sprite.vert", "../shaders/Sprite.frag", {0}),
      shader_sp_picking_("../shaders/Sprite.vert",
                         "../shaders/SpritePicking.frag", {}),
      shader_sp_hmap_("../shaders/Sprite.vert", "../shaders/SpriteHmap.frag", {0}),
      glfw_context_(global_glfw_callback_data) {
  Init();
}

void UiRenderData::Init() {
  InitVbos();
  InitVaos();
  shader_sp_mask_.Bind();
  glUniform1f(1, 1.0f);
  glUniform1f(2, 1.0f);
  shader_sp_.Bind();
  glUniform1f(1, 1.0f);
  glUniform1f(2, 1.0f);
  glUniform4fv(7, 1, glm::value_ptr(glm::vec4{1.0f}));
}

void UiRenderData::InitVbos() {
  GLuint vbos[3];
  glGenBuffers(3, vbos);
  vbo_ui_ = vbos[0];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER, data::kUiVboDataMain.size() * sizeof(float),
               data::kUiVboDataMain.data(), GL_DYNAMIC_DRAW);
  vbo_ui_transform_ = vbos[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_transform_);
  glBufferData(GL_ARRAY_BUFFER,
               data::kUiVboDataTransform.size() * sizeof(float),
               data::kUiVboDataTransform.data(), GL_DYNAMIC_DRAW);
}

void UiRenderData::InitVaos() {
  glGenVertexArrays(1, &vao_ui_);

  glBindVertexArray(vao_ui_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);  // position
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);  // tex coords

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_transform_);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(2);  // tex coords

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UiRenderData::DeInit() {
  glDeleteVertexArrays(1, &vao_ui_);
  GLuint vbos[2];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_ui_transform_;
  glDeleteBuffers(2, vbos);
}
