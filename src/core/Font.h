#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>
#include <stb_truetype.h>

#include "../render/Shader.h"
#include "../render/Texture.h"

class Font {
 public:
  Font();

  //  fill vbo -> render all collected text -> clear buffer for next frame
  void Render();

  /// add text instance to vbo data buffer
  void AddText(std::string_view text, glm::vec2 position_pix,
    glm::vec2 scale, glm::vec4 color);

  void AddText3D() {}

  glm::vec2 MeasureText(std::string_view text) const;

  static const int cMaxRenderChars;

  void AddSprite(const std::string& name, glm::vec2 position_pix,
    glm::vec2 size_pix, glm::vec4 color);

  void LoadAtlasInfo(const std::string& filepath);

 private:
  struct SpriteUV {
    glm::vec2 min_uv;
    glm::vec2 max_uv;
  };

  struct Vertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
  };

  void UpdateVbo();

  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  GLuint ebo_ = 0;

  std::vector<Vertex> vbo_data_text_;
  std::vector<stbtt_packedchar> packed_chars_;
  int mCharHeight = 0;
  Shader shader_text_;
  Texture texture_text_;

  Shader shader_ui_;
  Texture texture_ui_;
  std::vector<Vertex> vbo_data_ui_;
  std::unordered_map<std::string, SpriteUV> sprite_atlas_;
};
