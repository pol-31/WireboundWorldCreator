#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Vbos.h"
#include "ShadersBinding.h"

TextRenderer::TextRenderer(const Paths& paths)
    : texture_(paths.texture_text, GL_RGBA),
      shader_(paths.shader_text_vert, paths.shader_text_frag) {
  Init();
}

void TextRenderer::Init() {
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               vbos::kUiVboDataText.size() * sizeof(float),
               vbos::kUiVboDataText.data(), GL_STATIC_DRAW);

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

void TextRenderer::Bind() const {
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  texture_.Bind();
  shader_.Bind();
}

void TextRenderer::UnBind() {
  glBindVertexArray(0);
  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderDescription(int vbo_offset) const {
  if (vbo_offset != -1) {
    //TODO: add animation
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    texture_.Bind();
    shader_.Bind();
    float width = vbos::kUiVboDataText[vbo_offset * 2]
                  - vbos::kUiVboDataText[vbo_offset * 2 + 4];
    float height = vbos::kUiVboDataText[vbo_offset * 2 + 3]
                   - vbos::kUiVboDataText[vbo_offset * 2 + 1];
    glUniform2fv(shader::kTextScale, 1, glm::value_ptr(glm::vec2{width, height}));
    glUniform2fv(shader::kTextTranslate, 1, glm::value_ptr(glm::vec2{-0.0f, -0.8f}));
    glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
  }
}

void TextRenderer::RenderNumber(int number, glm::vec2 position) {
  std::string str = std::to_string(number);
  int total_length = 0;
  for (auto c : str) {
    total_length += vbos::GetTextWidth(vbos::GetTextVbo(c));
  }
  int render_offset = static_cast<int>(position.x) + (total_length + 1) / 2;
  for (auto c : str) {
    int vbo_offset = vbos::GetTextWidth(c);
    int c_half_length = vbos::GetTextWidth(vbo_offset);
    render_offset += c_half_length;
    glUniform2fv(shader::kTextScale, 1, glm::value_ptr(glm::vec2{1.0f, 1.0f}));
    glUniform2fv(shader::kTextTranslate, 1,
                 glm::value_ptr(glm::vec2{render_offset, position.y}));
    glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
    render_offset += c_half_length;
  }
}
