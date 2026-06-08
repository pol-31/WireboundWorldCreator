#include "Font.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include "ReadData.h"
#include "../io/Window.h"

const int Font::cMaxRenderChars = 1000;

Font::Font()
: mCharHeight(24),
  shader_text_("../shaders/Ui.vert", "../shaders/Text.frag", {0}),
  shader_ui_("../shaders/Ui.vert", "../shaders/Ui.frag", {0}),
  texture_ui_("../assets/TexAtlas.png", Texture::Type::UiAtlas) {
  std::vector<std::uint8_t> font_data = ReadData("../assets/Roboto-Regular.ttf");
  stbtt_fontinfo font;
  if (!stbtt_InitFont(&font, font_data.data(),
                      stbtt_GetFontOffsetForIndex(font_data.data(), 0)))
    throw std::runtime_error("unable to load font file");

  const int atlasWidth = 512;
  const int atlasHeight = 512;
  std::vector<uint8_t> pixels(atlasWidth * atlasHeight, 0);

  stbtt_pack_context pc;
  packed_chars_ = std::vector<stbtt_packedchar>(96);

  stbtt_PackBegin(&pc, pixels.data(), atlasWidth, atlasHeight, 0, 1, nullptr);
  stbtt_PackSetOversampling(&pc, 2, 2);
  stbtt_PackFontRange(&pc, font_data.data(), 0, mCharHeight, 32, 96, packed_chars_.data());
  stbtt_PackEnd(&pc);

  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight,
    0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  texture_text_ = Texture(texture_id, atlasWidth, atlasHeight, GL_RED, GL_R8, GL_UNSIGNED_BYTE);
  texture_text_.StoreImage("Bitmap.png", 1);

  int max_vertices = cMaxRenderChars * 4;
  int max_indices = cMaxRenderChars * 6;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  std::vector<GLuint> indices(max_indices);
  for (int i = 0; i < cMaxRenderChars; ++i) {
    uint32_t vOffset = i * 4;
    uint32_t iOffset = i * 6;

    indices[iOffset + 0] = vOffset + 0;
    indices[iOffset + 1] = vOffset + 1;
    indices[iOffset + 2] = vOffset + 2;

    indices[iOffset + 3] = vOffset + 0;
    indices[iOffset + 4] = vOffset + 2;
    indices[iOffset + 5] = vOffset + 3;
  }

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);

  LoadAtlasInfo("../assets/TexCoordsInfo.txt");
}


void Font::LoadAtlasInfo(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open atlas info file: " + filepath);
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;

    std::stringstream ss(line);
    std::string name, u0, v0, empty, u1, v1;

    // Parse using comma as the delimiter
    std::getline(ss, name, ',');
    std::getline(ss, u0, ',');
    std::getline(ss, v0, ',');
    std::getline(ss, u1, ',');
    std::getline(ss, v1, ',');

    sprite_atlas_[name] = {
      {std::stof(u0), std::stof(v0)},
      {std::stof(u1), std::stof(v1)}
    };
  }
}
void Font::UpdateVbo() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    int text_bytes = vbo_data_text_.size() * sizeof(Vertex);
    if (text_bytes > 0) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, text_bytes, vbo_data_text_.data());
    }
    int ui_bytes = vbo_data_ui_.size() * sizeof(Vertex);
    if (ui_bytes > 0) {
        glBufferSubData(GL_ARRAY_BUFFER, text_bytes, ui_bytes, vbo_data_ui_.data());
    }
}

void Font::Render() {
    if (vbo_data_text_.empty() && vbo_data_ui_.empty()) return;
    shader_text_.DebugUpdate();
    shader_ui_.DebugUpdate();
    UpdateVbo();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(gWindowWidth),
                                      static_cast<float>(gWindowHeight), 0.0f,
                                      -1.0f, 1.0f);
    glBindVertexArray(vao_);
    if (!vbo_data_text_.empty()) {
        shader_text_.Bind();
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projection));
        glActiveTexture(GL_TEXTURE0);
        texture_text_.BindSampler(0);
        GLsizei text_indices = static_cast<GLsizei>((vbo_data_text_.size() / 4) * 6);
        glDrawElements(GL_TRIANGLES, text_indices, GL_UNSIGNED_INT, nullptr);
    }
    if (!vbo_data_ui_.empty()) {
        shader_ui_.Bind();
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projection));
        glActiveTexture(GL_TEXTURE0);
        texture_ui_.BindSampler(0);
        GLsizei ui_indices = static_cast<GLsizei>((vbo_data_ui_.size() / 4) * 6);
        size_t text_index_count = (vbo_data_text_.size() / 4) * 6;
      // text_index_count = 0;
        void* ebo_byte_offset = reinterpret_cast<void*>(text_index_count * sizeof(GLuint));
        glDrawElements(GL_TRIANGLES, ui_indices, GL_UNSIGNED_INT, ebo_byte_offset);
    }
    glBindVertexArray(0);
    vbo_data_text_.clear();
    vbo_data_ui_.clear();
}

void Font::AddSprite(const std::string& name, glm::vec2 position_pix, glm::vec2 size_pix, glm::vec4 color) {
  auto it = sprite_atlas_.find(name);
  if (it == sprite_atlas_.end()) {
    std::cerr << "Warning: Sprite not found: " << name << "\n";
    return;
  }
  const SpriteUV& uv = it->second;

  float x0 = position_pix.x;
  float y0 = position_pix.y;
  float x1 = position_pix.x + size_pix.x;
  float y1 = position_pix.y + size_pix.y;

  Vertex v0{ {x0, y0}, {uv.min_uv.x, uv.max_uv.y}, color };
  Vertex v1{ {x0, y1}, {uv.min_uv.x, uv.min_uv.y}, color };
  Vertex v2{ {x1, y1}, {uv.max_uv.x, uv.min_uv.y}, color };
  Vertex v3{ {x1, y0}, {uv.max_uv.x, uv.max_uv.y}, color };

  vbo_data_ui_.push_back(v0);
  vbo_data_ui_.push_back(v1);
  vbo_data_ui_.push_back(v2);
  vbo_data_ui_.push_back(v3);
}

void Font::AddText(std::string_view text, glm::vec2 position_pix,
  glm::vec2 scale, glm::vec4 color) {
  if ((vbo_data_text_.size() / 4) + text.length() > static_cast<size_t>(cMaxRenderChars)) {
    return;
  }
  float cursor_x = 0.0f;
  float cursor_y = 0.0f;
  for (char c : text) {
    if (c < 32 || c > 126) continue;
    stbtt_aligned_quad q;
    stbtt_GetPackedQuad(packed_chars_.data(), texture_text_.GetWidth(), texture_text_.GetHeight(),
                        c - 32, &cursor_x, &cursor_y, &q, 1);

    float x0 = position_pix.x + q.x0 * scale.x;
    float y0 = position_pix.y + q.y0 * scale.y;
    float x1 = position_pix.x + q.x1 * scale.x;
    float y1 = position_pix.y + q.y1 * scale.y;

    Vertex v0{ {x0, y0}, {q.s0, q.t0}, color };
    Vertex v1{ {x0, y1}, {q.s0, q.t1}, color };
    Vertex v2{ {x1, y1}, {q.s1, q.t1}, color };
    Vertex v3{ {x1, y0}, {q.s1, q.t0}, color };

    vbo_data_text_.push_back(v0);
    vbo_data_text_.push_back(v1);
    vbo_data_text_.push_back(v2);
    vbo_data_text_.push_back(v3);
  }
}

glm::vec2 Font::MeasureText(std::string_view text) const {
  float cursor_x = 0.0f;
  float cursor_y = 0.0f;

  float min_y = 0.0f;
  float max_y = 0.0f;

  for (char c : text) {
    if (c < 32 || c > 126) continue;

    stbtt_aligned_quad q;
    stbtt_GetPackedQuad(packed_chars_.data(), texture_text_.GetWidth(), texture_text_.GetHeight(),
                        c - 32, &cursor_x, &cursor_y, &q, 1);

    if (q.y0 < min_y) min_y = q.y0;
    if (q.y1 > max_y) max_y = q.y1;
  }

  // cursor_x contains the final total horizontal displacement width
  float total_width = cursor_x;
  float total_height = max_y - min_y;

  return glm::vec2(total_width, total_height);
}
