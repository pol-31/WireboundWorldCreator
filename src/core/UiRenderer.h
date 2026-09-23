#ifndef WIREBOUNDWORLDCREATOR_UIRENDERER_H
#define WIREBOUNDWORLDCREATOR_UIRENDERER_H

#include <glm/glm.hpp>
#include <stb_truetype.h>

#include "../render/Shader.h"
#include "../render/Texture.h"

class UiRenderer {
public:
  UiRenderer();

  //  fill vbo -> render all collected text -> clear buffer for next frame
  void Render();

  void LoadAtlasInfo(const std::string& filepath);

  [[nodiscard]] int GetCharHeight() const noexcept {
    return mCharHeight;
  }

  void AddText(std::string_view text, glm::vec2 position_pix,
    glm::vec2 scale, glm::vec4 color);

  glm::vec2 MeasureText(std::string_view text) const;

  void AddSprite(const std::string& name, glm::vec2 position_pix,
    glm::vec2 size_pix, glm::vec4 color);

  static const int cMaxRenderChars;

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


  //Font:
  std::vector<stbtt_packedchar> packed_chars_;
  Texture texture_text_;
  int mCharHeight = 0;

  //SpriteAtlas:
  std::unordered_map<std::string, SpriteUV> sprite_atlas_;
  Texture texture_ui_;

  Shader shader_text_;
  std::vector<Vertex> vbo_data_text_;

  Shader shader_ui_;
  std::vector<Vertex> vbo_data_ui_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIRENDERER_H
