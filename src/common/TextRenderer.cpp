#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <stb_image_write.h>

#include "../common/Font.h"
#include "../common/Text.h"
#include "ShadersBinding.h"
#include "../io/Window.h"

void TextRendererCharCallback(GLFWwindow* window, unsigned int codepoint) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  // carriage return || escape
  if (codepoint == 13 || codepoint == 27) {
    global_data->StopCharInput();
  }
  // > whitespace or < del (printable char)
  if (codepoint > 31 && codepoint < 127) {
    global_data->text_renderer_->AppendChar(codepoint);
  }
  // dbg
  std::cout << codepoint << std::endl;
}

void TextRendererMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    global_data->StopCharInput();
  }
}

TextRenderer::TextRenderer(UiDynamicSprite&& text_slot, const Paths& paths)
    : tex_bitmap_("../assets/bmp_ascii_header.png", GL_RED),
//    : tex_bitmap_("../assets/AsciiBitmap.png", GL_RED),
      tex_menu_(1024, 1024, GL_RED),
      tex_mode_(1024, 1024, GL_RED),
      render_shader_(
          paths.shader_text_vert, paths.shader_text_frag),
      render_shader_picking_(
          paths.shader_text_vert, paths.shader_sprite_picking_frag),
      text_slot_(std::move(text_slot)),
      scale_(0.5) {
  Init();
}

TextRenderer::~TextRenderer() {
  DeInit();
}

void TextRenderer::Init() {
  render_shader_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  glUniform1f(shader::kSpriteTransparency, 1.0f);
  glUniform1f(shader::kSpriteResolution, gResFactor);
  render_shader_picking_.Bind();
  glUniform1i(shader::kSpriteTexture, 0);
  glUniform1f(shader::kSpriteResolution, gResFactor);
  glUseProgram(0);

  GLuint fbos[2];
  glGenFramebuffers(2, fbos);
  fbo_read_id_ = fbos[0];
  fbo_write_id_ = fbos[1];

  SetupFramebuffer(fbo_read_id_, tex_bitmap_, false);
}

void TextRenderer::DeInit() {
  GLuint fbos[2] = {fbo_read_id_, fbo_write_id_};
  glDeleteFramebuffers(2, fbos);
}

void TextRenderer::RenderMenuText(int id) {
  glActiveTexture(GL_TEXTURE0);
  tex_menu_.Bind();
  render_shader_.Bind();
  glUniform3f(4, 0.2118f, 0.1647f, 0.0745f);

  Aabb coords = coords_menu_[id];
  auto coords_transform = CoordsToTransformMatrix(coords);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  int width = coords.right - coords.left;
  int height = coords.top - coords.bottom;
  text_slot_.SetExtraScale(static_cast<float>(width) / height);
  // translation & rotation the same

  text_slot_.Render();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderMenuTextPicking(int id) {
  glActiveTexture(GL_TEXTURE0);
  tex_menu_.Bind();
  render_shader_picking_.Bind();

  auto coords_transform = CoordsToTransformMatrix(coords_menu_[id]);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  text_slot_.RenderPicking();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderModeText(int id) {
  glActiveTexture(GL_TEXTURE0);
  tex_mode_.Bind();
  render_shader_.Bind();
  glUniform3f(4, 0.2118f, 0.1647f, 0.0745f);

  Aabb coords = coords_mode_[id];
  auto coords_transform = CoordsToTransformMatrix(coords);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  int width = coords.right - coords.left;
  int height = coords.top - coords.bottom;
  text_slot_.SetExtraScale(static_cast<float>(width) / height);
  // translation & rotation the same

  text_slot_.Render();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderModeTextPicking(int id) {
  glActiveTexture(GL_TEXTURE0);
  tex_mode_.Bind();
  render_shader_picking_.Bind();

  auto coords_transform = CoordsToTransformMatrix(coords_mode_[id]);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  text_slot_.RenderPicking();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::SetupFramebuffer(
    GLuint fbo_id, Texture& texture, bool clear) {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture.GetId(), 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer is not complete!");
  }

  if (clear) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int GetWidth(int code) {
  float comp_width;
  if (code & 1) {
    comp_width = static_cast<float>((font::gWidths[code >> 1]) >> 4);
  } else {
    comp_width = static_cast<float>((font::gWidths[code >> 1]) & 0x0F);
  }
  auto result = static_cast<int>(comp_width * font::gWidthFactor) + font::gWidthMin;
  return result;
}

int TextRenderer::CalculateLength(std::string_view text) {
  int total_length = 0;
  for (auto ch : text) {
    total_length += GetWidth(static_cast<int>(ch) - 32);
  }
  return total_length * scale_;
}

void TextRenderer::PrerenderMenuText(int start, int end) {
  PrerenderImpl(start, end, tex_menu_, coords_menu_);
}

void TextRenderer::PrerenderModeText(int start, int end) {
  PrerenderImpl(start, end, tex_mode_, coords_mode_);
}

void TextRenderer::RenderText(
    /*UiDynamicSprite& text_slot, */std::string_view text,
    float scale, glm::vec2 position) {
  float calculated_width = CalculateLength(text) / 1024.0f;
  float symbol_height = font::gFullHeight * scale_ / 1024.0f;

  glActiveTexture(GL_TEXTURE0);
  tex_bitmap_.Bind();
  render_shader_.Bind();
  glUniform3f(4, 0.0f, 0.0f, 0.0f);
//  glUniform3f(4, 0.55f, 0.4f, 0.005f);

  glm::vec2 next_pos{-calculated_width / 2.0f, symbol_height};
  next_pos += position;

  text_slot_.SetScale(scale);

  for (auto ch : text) {
    if (ch == '\n') {
      next_pos.y -= symbol_height;
      next_pos.x = -calculated_width / 2.0f + position.x;
      continue;
    }
    auto coords = GetGlyphCoords(ch);
    auto coords_transform = CoordsToTransformMatrix(coords);
    float symbol_width = GetWidth(static_cast<int>(ch) - 32) * scale_ / 1024.0f;
    glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
    text_slot_.SetExtraScale(
        static_cast<float>(GetWidth(static_cast<int>(ch) - 32)) / font::gFullHeight);
    text_slot_.SetTranslate(next_pos);
    text_slot_.Render();

    next_pos.x += symbol_width;
  }

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(
    const FixedSizeQueue<char, 64>* text,
    float scale, glm::vec2 position) {
  RenderText(std::string_view(text->cbegin(), text->cend()), scale, position);
}

void TextRenderer::RenderTextPicking(
    /*UiDynamicSprite& text_slot, */std::string_view text,
    float scale, glm::vec2 position) {
  float calculated_width = CalculateLength(text) / 1024.0f;
  float symbol_height = font::gFullHeight * scale_ / 1024.0f;

  glActiveTexture(GL_TEXTURE0);
  tex_bitmap_.Bind();
  render_shader_picking_.Bind();

  text_slot_.SetScale(calculated_width);

  glm::mat3 coords_transform{1.0f}; // full screen
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));

  text_slot_.SetExtraScale(calculated_width / symbol_height);
  text_slot_.SetTranslate(position);

  text_slot_.RenderPicking();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderTextPicking(
    const FixedSizeQueue<char, 64>* text,
    float scale, glm::vec2 position) {
  RenderTextPicking(std::string_view(text->cbegin(), text->cend()),
                    scale, position);
}

void TextRenderer::PrerenderImpl(
    int start, int end, Texture& texture, std::vector<Aabb>& coords) {
  SetupFramebuffer(fbo_write_id_, texture, true);
  fbo_cursor_ = glm::ivec2{0, 1024};

  int size = end - start;
  coords.reserve(size);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_read_id_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_write_id_);
  for (int i = start; i < end; ++i) {
    coords.push_back(RenderPhrase(text::gText[i]));
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// only 1-row text by now
TextRenderer::Aabb TextRenderer::RenderPhrase(std::string_view text) {
  int calculated_width = CalculateLength(text);
  int symbol_height = static_cast<int>(font::gFullHeight * scale_);
  if (fbo_cursor_.x + calculated_width > 1024) {
    fbo_cursor_.x = 0;
    fbo_cursor_.y -= symbol_height;
  }
  if (fbo_cursor_.y - symbol_height < 0) {
    std::cerr << "TextRenderer overflow on Y" << std::endl;
    fbo_cursor_.y = 1024;
  }
  Aabb aabb = {fbo_cursor_.x, fbo_cursor_.x + calculated_width, fbo_cursor_.y,
               fbo_cursor_.y - symbol_height};
  for (auto ch : text) {
    // affects fbo_cursor_.x
    RenderSymbol(ch);
  }
  return aabb;
}

void TextRenderer::RenderSymbol(char ch) {
  Aabb src = GetGlyphCoords(ch);
  Aabb dst = {fbo_cursor_.x,
              fbo_cursor_.x + static_cast<int>(
                                  GetWidth(static_cast<int>(ch) - 32) * scale_),
              fbo_cursor_.y,
              fbo_cursor_.y - static_cast<int>(font::gFullHeight * scale_)};
  fbo_cursor_.x += static_cast<int>(
      GetWidth(static_cast<int>(ch) - 32) * scale_);
  if (static_cast<int>(ch) == 106) {
//    std::cout << std::endl;
  }
  glBlitFramebuffer(src.left, src.bottom, src.right, src.top,
                    dst.left, dst.bottom, dst.right, dst.top,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

TextRenderer::Aabb TextRenderer::GetGlyphCoords(char ch) {
  int id = static_cast<int>(ch);
  int column = id & 7;
  int local_id = id - 32;  // bitmap starts from 32
  int row = 12 - (static_cast<int>(local_id / 8));
  return {128 * column,
          128 * column + GetWidth(local_id),
          85 * (row),
          85 * (row - 1)};
}

glm::mat3 TextRenderer::CoordsToTransformMatrix(TextRenderer::Aabb aabb) {
  float w = (aabb.right - aabb.left) / 1024.0f;
  float h = (aabb.top - aabb.bottom) / 1024.0f;
  glm::vec2 scale{w, h};
  glm::vec2 translate{aabb.left / 1024.0f, aabb.bottom / 1024.0f};
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, translate);
  transform = glm::scale(transform, scale);
  return transform;
}
