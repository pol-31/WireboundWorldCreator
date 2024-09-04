#include "SharedResources.h"

#include "../common/Vbos.h"
#include "../core/TileRenderer.h"
#include "../common/ShadersBinding.h"

SharedResources::SharedResources(
    const Paths& paths, Tile& cur_tile, TileRenderer& tile_renderer_,
    GlobalGlfwCallbackData& global_glfw_callback_data)
    : tex_ui_(paths.texture_ui, GL_RGBA),
      static_sprite_shader_(paths.shader_static_sprite_vert,
                           paths.shader_static_sprite_frag),
      menu_icon_shader_(paths.shader_static_sprite_vert,
                        paths.menu_icon_shader),
      global_glfw_callback_data_(global_glfw_callback_data),
      static_sprite_picking_shader_(paths.shader_static_sprite_pick_vert,
                                   paths.shader_static_sprite_pick_frag),
      dynamic_sprite_shader_(paths.shader_dynamic_sprite_vert,
                             paths.shader_dynamic_sprite_frag),
      tile_renderer_(tile_renderer_),
      tile_(cur_tile) {
  Init();
}

void SharedResources::Init() {
  InitVbos();
  InitVaos();
  glActiveTexture(GL_TEXTURE0);
  tex_ui_.Bind();
  static_sprite_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  menu_icon_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  dynamic_sprite_shader_.Bind();
  glUniform1i(shader::kDynamicSpriteTexture, 0);
  static_sprite_picking_shader_.Bind();
  glUniform1i(shader::kSpritePickingTexture, 0);
}

void SharedResources::InitVbos() {
  GLuint vbos[2];
  glGenBuffers(2, vbos);
  vbo_ui_ = vbos[0];
  vbo_instanced_ = vbos[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataMain.size() * sizeof(float),
               details::kUiVboDataMain.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataInstanced.size() * sizeof(float),
               details::kUiVboDataInstanced.data(), GL_STATIC_DRAW);
}

void SharedResources::InitVaos() {
  glGenVertexArrays(1, &vao_ui_);

  glBindVertexArray(vao_ui_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0); // position
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1); // tex coords

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SharedResources::DeInit() {
  DeInitVbos();
  DeInitVaos();
}

void SharedResources::DeInitVaos() {
  glDeleteVertexArrays(1, &vao_ui_);
}

void SharedResources::DeInitVbos() const {
  GLuint vbos[2];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_instanced_;
  glDeleteBuffers(2, vbos);
}
