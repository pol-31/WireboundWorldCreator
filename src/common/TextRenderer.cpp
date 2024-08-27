#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Vbos.h"
#include "ShadersBinding.h"

TextRenderer::TextRenderer(const Paths& paths)
    : texture_(paths.texture_text),
      shader_(paths.shader_text_vert, paths.shader_text_frag) {
  Init();
}

void TextRenderer::Init() {
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               details::kUiVboDataText.size() * sizeof(float),
               details::kUiVboDataText.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  // we use uniform position (specified direcly in shader Text.frag)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0); // tex coords

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0); // TODO: GL_TEXTURE1
  texture_.Bind();
  shader_.Bind();
  glUniform1i(shader::kTextTexture, 0);
}

const Shader& TextRenderer::Bind() const {
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  texture_.Bind();
  shader_.Bind();
  return shader_;
}

void TextRenderer::UnBind() {
  glBindVertexArray(0);
  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderDescription(int vbo_offset) {
  if (vbo_offset != -1) {
    //TODO: add animation
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    texture_.Bind();
    shader_.Bind();
    float width = details::kUiVboDataText[vbo_offset * 2]
                  - details::kUiVboDataText[vbo_offset * 2 + 4];
    float height = details::kUiVboDataText[vbo_offset * 2 + 3]
                   - details::kUiVboDataText[vbo_offset * 2 + 1];
    glUniform2fv(shader::kTextScale, 1, glm::value_ptr(glm::vec2{width, height}));
    glUniform2fv(shader::kTextTranslate, 1, glm::value_ptr(glm::vec2{-0.0f, -0.8f}));
    glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
  }
}
