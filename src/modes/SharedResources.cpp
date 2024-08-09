#include "SharedResources.h"

#include "../vbos/AllVbos.h"
#include "../core/TileRenderer.h"

SharedResources::SharedResources(
    const Paths& paths, TileRenderer& tile_renderer_,
    GlobalGlfwCallbackData& global_glfw_callback_data_)
    : tex_text_(paths.texture_text),
      tex_ui_(paths.texture_ui),
      static_sprite_shader(paths.shader_static_sprite_vert,
                           paths.shader_static_sprite_frag),
      slider_handle_shader(paths.shader_slider_handle_vert,
                           paths.shader_slider_handle_frag),
      tile_renderer(tile_renderer_),
      global_glfw_callback_data(global_glfw_callback_data_) {
  Init();
}

void SharedResources::Init() {
  InitVbos();
  InitVaos();
  glActiveTexture(GL_TEXTURE0);
  tex_ui_.Bind();
  glActiveTexture(GL_TEXTURE1);
  tex_text_.Bind();
  static_sprite_shader.Bind();
  static_sprite_shader.SetUniform("tex", 0);
  slider_handle_shader.Bind();
  slider_handle_shader.SetUniform("tex", 0);
}

void SharedResources::InitVbos() {
  GLuint vbos[3];
  glGenBuffers(3, vbos);
  vbo_ui_ = vbos[0];
  vbo_text_ = vbos[1];
  vbo_instanced_ = vbos[2];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataMain.size() * sizeof(float),
               details::kUiVboDataMain.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_text_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataText.size() * sizeof(float),
               details::kUiVboDataText.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataInstanced.size() * sizeof(float),
               details::kUiVboDataInstanced.data(), GL_STATIC_DRAW);
}

void SharedResources::InitVaos() {
  GLuint vaos[2];
  glGenVertexArrays(2, vaos);
  vao_ui_ = vaos[0]; // pos/tex_coords both interleaving
  vao_text_ = vaos[1]; // 1 pos from vbo1 + n tex_coords from vbo2

  glBindVertexArray(vao_ui_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0); // position
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1); // tex coords
/*
 * TODO: here we use not VertexArray, but uniform variable and start point:
 *  we want to set position according to message size (to center it)
  glBindVertexArray(vao_text_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  // we don't update it, use uniform position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0,
                        reinterpret_cast<void*>(TODO:HERE_OUR_UNIFORM_POS));
  glEnableVertexAttribArray(0); // position
  glBindBuffer(GL_ARRAY_BUFFER, vbo_text_);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(1); // tex coords*/
}

void SharedResources::DeInit() {
  DeInitVbos();
  DeInitVaos();
}

void SharedResources::DeInitVaos() {
  GLuint vaos[2];
  vaos[0] = vao_ui_;
  vaos[1] = vao_text_;
  glDeleteVertexArrays(2, vaos);
}

void SharedResources::DeInitVbos() {
  GLuint vbos[3];
  vbos[0] = vbo_ui_;
  vbos[1] = vbo_text_;
  vbos[2] = vbo_instanced_;
  glDeleteBuffers(3, vbos);
}
