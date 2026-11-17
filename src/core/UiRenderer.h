#ifndef WIREBOUNDWORLDCREATOR_UIRENDERER_H
#define WIREBOUNDWORLDCREATOR_UIRENDERER_H

#include <stb_truetype.h>

#include <glm/glm.hpp>

#include "../render/Shader.h"
#include "../render/Texture.h"
#include "../render/SpriteUv.h"

class UiRenderer {
public:
  struct Vertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
  };

  UiRenderer();

  ~UiRenderer() {
    DeInit();
  }

  //  fill vbo -> render all collected text -> clear buffer for next frame
  void Render();

  void LoadAtlasInfo(const std::string& filepath);

  [[nodiscard]] int GetCharHeight() const noexcept { return mCharHeight; }

  void AddText(std::string_view text, glm::vec2 position_pix, glm::vec2 scale,
               glm::vec4 color);

  glm::vec2 MeasureText(std::string_view text) const;

  void AddSprite(std::string_view name, glm::vec2 position_pix,
                 glm::vec2 size_pix, glm::vec4 color);

  SpriteUv GetSpriteUvs(const std::string& name);

  GLuint GetTexId() const noexcept {
    return texture_ui_.GetId();
  }

  static const int cMaxRenderChars;

 private:
  void Init();

  void DeInit();

  void UpdateVbo();

  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  GLuint ebo_ = 0;

  // Font:
  std::vector<stbtt_packedchar> packed_chars_;
  Texture texture_text_;
  int mCharHeight = 0;
  float font_ascent_ = 0.0f;;
  float font_descent_ = 0.0f;
  float font_line_gap_ = 0.0f;
  float font_line_height_ = 0.0f;
  float font_middle_offset_ = 0.0f;

  // SpriteAtlas:
  std::unordered_map<std::string, SpriteUv> sprite_atlas_;
  Texture texture_ui_;

  Shader shader_text_;
  std::vector<Vertex> vbo_data_text_;

  Shader shader_ui_;
  std::vector<Vertex> vbo_data_ui_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIRENDERER_H
