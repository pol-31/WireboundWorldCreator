#include "UiSharedResources.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common/Vbos.h"
#include "../io/Window.h"

UiSharedResources::UiSharedResources(GlfwContext& global_glfw_callback_data)
    : tex_ui_("../assets/TexAtlas.png", GL_RGBA),
      tex_ui_mask_("../assets/tex_ui_mask.png", GL_RGBA),
      shader_sp_mask_("../shaders/Sprite.vert", "../shaders/SpriteMask.frag"),
      shader_sp_("../shaders/Sprite.vert", "../shaders/Sprite.frag"),
      shader_sp_picking_("../shaders/Sprite.vert",
                         "../shaders/SpritePicking.frag"),
      shader_sp_hmap_("../shaders/Sprite.vert", "../shaders/SpriteHmap.frag"),
      glfw_context_(global_glfw_callback_data),
      shader_mdl_("../shaders/Model.vert", "../shaders/Model.frag"),
      shader_animated_mdl_("../shaders/ModelAnimated.vert",
                           "../shaders/Model.frag"),
      shader_mdl_color_("../shaders/Model.vert", "../shaders/ModelColor.frag"),
      shader_mdl_instanced_("../shaders/ModelInstanced.vert",
                            "../shaders/Model.frag"),
      shader_mdl_picking_("../shaders/ModelPicking.vert",
                          "../shaders/ModelPicking.frag"),
      shader_animated_mdl_picking_("../shaders/ModelAnimatedPicking.vert",
                                   "../shaders/ModelPicking.frag"),
      shader_mdl_selected_("../shaders/ModelPicking.vert",
                           "../shaders/ModelSelected.frag"),
      shader_animated_mdl_selected_("../shaders/ModelAnimatedPicking.vert",
                                    "../shaders/ModelSelected.frag") {
  Init();
}

void UiSharedResources::Init() {
  InitVbos();
  InitVaos();
  shader_sp_mask_.Bind();
  glUniform1i(0, 0);
  glUniform1f(1, 1.0f);
  glUniform1f(2, 1.0f);
  glUniform1i(6, 1);

  shader_sp_.Bind();
  glUniform1i(0, 0);
  glUniform1f(1, 1.0f);
  glUniform1f(2, 1.0f);
  glUniform4fv(7, 1, glm::value_ptr(glm::vec4{1.0f}));

  shader_sp_hmap_.Bind();
  glUniform1i(0, 0);

  shader_mdl_.Bind();
  glUniform1i(1, 0);
  shader_animated_mdl_.Bind();
  glUniform1i(1, 0);
  shader_mdl_instanced_.Bind();
  glUniform1i(1, 0);
}

void UiSharedResources::InitVbos() {
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

void UiSharedResources::InitVaos() {
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

void UiSharedResources::DeInit() {
  glDeleteVertexArrays(1, &vao_ui_);
  GLuint vbos[2];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_ui_transform_;
  glDeleteBuffers(2, vbos);
}
