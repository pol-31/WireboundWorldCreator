#include "SharedResources.h"

#include "../common/Vbos.h"
#include "../core/TileRenderer.h"
#include "../common/ShadersBinding.h"

SharedResources::SharedResources(
    const Paths& paths, Tile& cur_tile, TileRenderer& tile_renderer_,
    GlobalGlfwCallbackData& global_glfw_callback_data)
    : tex_ui_(paths.texture_ui, GL_RGBA),
      tex_ui_mask_(paths.texture_ui_mask, GL_RGBA),
      static_sprite_shader_(paths.shader_sprite_static_vert,
                           paths.shader_sprite_frag),
      menu_icon_shader_(paths.shader_sprite_static_vert,
                        paths.menu_icon_shader),
      static_sprite_picking_shader_(paths.shader_sprite_static_vert,
                                   paths.shader_sprite_picking_frag),
      mask_sprite_shader_(paths.shader_sprite_static_vert,
                          paths.shader_sprite_mask_frag),
      dynamic_sprite_shader_(paths.shader_sprite_dynamic_vert,
                             paths.shader_sprite_frag),
      dynamic_sprite_picking_shader_(paths.shader_sprite_dynamic_vert,
                                     paths.shader_sprite_picking_frag),
      static_sprite_progress_shader_(paths.shader_sprite_progress_vert,
                                     paths.shader_sprite_frag),
      global_glfw_callback_data_(global_glfw_callback_data),
      tile_renderer_(tile_renderer_),
      tile_(cur_tile) {
  Init();
}

void SharedResources::UpdateResolution() {
  float res_factor = static_cast<float>(gWindowHeight) /
                     static_cast<float>(gWindowWidth);
  static_sprite_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);

  //Good!
  static_sprite_picking_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);
  dynamic_sprite_picking_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);

//  static_sprite_progress_shader_.Bind();
//  glUniform1f(shader::kSpriteResolution, res_factor);
//  menu_icon_shader_.Bind();
//  glUniform1f(shader::kSpriteResolution, res_factor);


  mask_sprite_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);

  dynamic_sprite_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);
  glUseProgram(0);
}

void SharedResources::Init() {
  InitVbos();
  InitVaos();
  float res_factor = static_cast<float>(gWindowHeight) /
                     static_cast<float>(gWindowWidth);
  static_sprite_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  glUniform1f(shader::kSpriteTransparency, 1.0f);
  glUniform1f(shader::kSpriteResolution, res_factor);
  static_sprite_progress_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  glUniform1f(shader::kSpriteTransparency, 1.0f);

  //Good!
  static_sprite_picking_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);
  dynamic_sprite_picking_shader_.Bind();
  glUniform1f(shader::kSpriteResolution, res_factor);

//  glUniform1f(shader::kSpriteResolution, res_factor);
  menu_icon_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
//  glUniform1f(shader::kSpriteResolution, res_factor);

  mask_sprite_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  glUniform1f(shader::kSpriteTransparency, 1.0f);
  glUniform1f(shader::kSpriteResolution, res_factor);
  glUniform1i(shader::kSpriteMask, 1);

  dynamic_sprite_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  glUniform1f(shader::kSpriteTransparency, 1.0f);
  glUniform1f(shader::kSpriteResolution, res_factor);
  glUseProgram(0);
//  static_sprite_picking_shader_.Bind();
//  glUniform1i(shader::kSpriteTexture, 0);
//  dynamic_sprite_picking_shader_.Bind();
//  glUniform1i(shader::kSpriteTexture, 0);
}

void SharedResources::InitVbos() {
#ifndef NDEBUG
  GLuint vbos[3];
  glGenBuffers(3, vbos);
  vbo_ui_ = vbos[0];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER,
               vbos::kUiVboDataMain.size() * sizeof(float),
               vbos::kUiVboDataMain.data(), GL_DYNAMIC_DRAW);
  vbo_ui_transform_ = vbos[2];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_transform_);
  glBufferData(GL_ARRAY_BUFFER,
               vbos::kUiVboDataTransform.size() * sizeof(float),
               vbos::kUiVboDataTransform.data(), GL_DYNAMIC_DRAW);
#else
  GLuint vbos[2];
  glGenBuffers(2, vbos);
  vbo_ui_ = vbos[0];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER,
               vbos::kUiVboDataMain.size() * sizeof(float),
               vbos::kUiVboDataMain.data(), GL_STATIC_DRAW);
#endif // NDEBUG

  vbo_instanced_ = vbos[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced_);
  glBufferData(GL_ARRAY_BUFFER,
               vbos::kUiVboDataInstanced.size() * sizeof(float),
               vbos::kUiVboDataInstanced.data(), GL_STATIC_DRAW);
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

#ifndef NDEBUG
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_transform_);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(2); // tex coords
#endif // NDEBUG

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
#ifndef NDEBUG
  GLuint vbos[3];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_instanced_;
  vbos[2] = vbo_ui_transform_;
  glDeleteBuffers(3, vbos);
#else
  GLuint vbos[2];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_instanced_;
  glDeleteBuffers(2, vbos);
#endif // NDEBUG
}
