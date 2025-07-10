#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <stb_image_write.h>

#include "../modes/IUiMode.h"
#include "../common/Font.h"
#include "../common/Text.h"
#include "ShadersBinding.h"
#include "../io/Window.h"
#include "UiDebugger.h"

TextRenderer::TextRenderer(const Paths& paths)
    : tex_bitmap_("../assets/bmp_ascii_header.png", GL_RED),
//    : tex_bitmap_("../assets/AsciiBitmap.png", GL_RED),
      tex_menu_(1024, 1024, GL_RED),
      tex_mode_(1024, 1024, GL_RED),
      render_shader_(
          paths.shader_text_vert, paths.shader_text_frag),
      render_shader_picking_(
          paths.shader_text_vert, paths.shader_sprite_picking_frag),
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
//  glUniform1f(shader::kSpriteResolution, gResFactor);
  render_shader_picking_.Bind();
//  glUniform1i(shader::kSpriteTexture, 0);
//  glUniform1f(shader::kSpriteResolution, gResFactor);
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

void TextRenderer::RenderMenuText(data::TextId id) {
  glActiveTexture(GL_TEXTURE0);
  tex_menu_.Bind();
  render_shader_.Bind();
  glUniform3f(4, 0.2118f, 0.1647f, 0.0745f);

  Aabb coords = coords_menu_[static_cast<int>(id)];
  auto coords_transform = CoordsToTransformMatrix(coords);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  int width = coords.right - coords.left;
  int height = coords.top - coords.bottom;
  text_slot_->SetExtraScale(static_cast<float>(width) / height);
  // translation & rotation the same

  text_slot_->Render();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderMenuTextPicking(data::TextId id) {
  glActiveTexture(GL_TEXTURE0);
  tex_menu_.Bind();
  render_shader_picking_.Bind();

  auto coords_transform = CoordsToTransformMatrix(
      coords_menu_[static_cast<int>(id)]);
//  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  text_slot_->RenderPicking();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderModeText(data::TextId id) {
  glActiveTexture(GL_TEXTURE0);
  tex_mode_.Bind();
  render_shader_.Bind();
  glUniform3f(4, 0.2118f, 0.1647f, 0.0745f);

  Aabb coords = coords_mode_[static_cast<int>(id)];
  auto coords_transform = CoordsToTransformMatrix(coords);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  int width = coords.right - coords.left;
  int height = coords.top - coords.bottom;
  text_slot_->SetExtraScale(static_cast<float>(width) / height);
  // translation & rotation the same

  text_slot_->Render();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderModeTextPicking(data::TextId id) {
  glActiveTexture(GL_TEXTURE0);
  tex_mode_.Bind();
  render_shader_picking_.Bind();

  auto coords_transform = CoordsToTransformMatrix(
      coords_mode_[static_cast<int>(id)]);
//  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  text_slot_->RenderPicking();

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

int TextRenderer::CalculateLineLength(
    std::string_view text, const UiDynamicSprite& text_slot) {
  int total_length = 0;
  for (auto ch : text) {
    if (ch == '\n') {
      break;
    }
    total_length += GetWidth(static_cast<int>(ch) - 32);
  }
  float ui_scale = debug::gUiTransforms[
                       4 * (text_slot.GetId() - details::kIdOffsetUi)
  ].scale;
  return total_length * scale_ * ui_scale * 1024.0f / 78.0f;
}

void TextRenderer::PrerenderMenuText(int start, int end) {
  PrerenderImpl(start, end, tex_menu_, coords_menu_);
}

void TextRenderer::PrerenderModeText(int start, int end) {
  PrerenderImpl(start, end, tex_mode_, coords_mode_);
}

void TextRenderer::RenderText(
    UiDynamicSprite& text_slot, std::string_view text,
    float scale, glm::vec2 position, Alignment alignment) {
  float symbol_height = font::gFullHeight * scale_ * scale / 1024.0f;

  glActiveTexture(GL_TEXTURE0);
  tex_bitmap_.Bind();
  render_shader_.Bind();

  text_slot.SetScale(scale * scale_);

  float ui_scale = debug::gUiTransforms[
                       4 * (text_slot.GetId() - details::kIdOffsetUi)
  ].scale;

  size_t line_start = 0;
  glm::vec2 next_pos{0.0f, position.y + symbol_height};

  float new_line_offset_factor;
  switch (alignment) {
    case Alignment::kCentre:
      new_line_offset_factor = -0.5f;
      break;
    case Alignment::kLeft:
      new_line_offset_factor = 0.0f;
      break;
    case Alignment::kRight:
      new_line_offset_factor = -1.0f;
      break;
  }

  while (line_start != std::string_view::npos && line_start < text.size()) {
    size_t line_end = text.find_first_of('\n', line_start);
    size_t length;
    if (line_end == std::string_view::npos) {
      length = text.size() - line_start;
    } else {
      length = line_end - line_start;
    }
    std::string_view line = text.substr(line_start, length);

    // --- process line ---
    float line_length = scale * CalculateLineLength(line, text_slot) / 1024.0f;

    next_pos.x = position.x + line_length * new_line_offset_factor;

    // --- process char ---

    for (auto ch : line) {
      auto coords = GetGlyphCoords(ch);
      auto coords_transform = CoordsToTransformMatrix(coords);
      float symbol_width = scale * scale_ * static_cast<float>(GetWidth(static_cast<int>(ch) - 32)) / 1024.0f;

      glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));

      text_slot.SetExtraScale(symbol_width / symbol_height);
      symbol_width *= ui_scale * 1024.0f / 78.0f;

      /// correct alignment (half prev, half next char)
      next_pos.x += symbol_width / 2.0f;
      text_slot.SetTranslate(next_pos);
      next_pos.x += symbol_width / 2.0f;

      text_slot.Render();
    }

    if (line_end == std::string_view::npos) {
      break;
    }
    line_start = line_end + 1;
    next_pos.y -= symbol_height * ui_scale * 1024.0f / (78.0f * 8 / 12);
  }

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(
    UiDynamicSprite& text_slot,
    const FixedSizeQueue<char, 64>* text,
    float scale, glm::vec2 position, Alignment alignment) {
  RenderText(text_slot, std::string_view(text->cbegin(), text->cend()),
             scale, position, alignment);
}

void TextRenderer::RenderTextPicking(
    UiDynamicSprite& text_slot, std::string_view text,
    float scale, glm::vec2 position, Alignment alignment) {
  /// picking for the first line is enough
  float symbol_height = font::gFullHeight * scale_ * scale / 1024.0f;

  glActiveTexture(GL_TEXTURE0);
  tex_bitmap_.Bind();
  render_shader_picking_.Bind();

  // not used
//  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));

  text_slot.SetScale(scale * scale_);

  float ui_scale = debug::gUiTransforms[
                       4 * (text_slot.GetId() - details::kIdOffsetUi)
  ].scale;

  size_t line_start = 0;
  glm::vec2 next_pos{0.0f, position.y + symbol_height};

  float new_line_offset_factor;
  switch (alignment) {
    case Alignment::kCentre:
      new_line_offset_factor = 0.0f;
      break;
    case Alignment::kLeft:
      new_line_offset_factor = 0.5f;
      break;
    case Alignment::kRight:
      new_line_offset_factor = -0.5f;
      break;
  }

  while (line_start != std::string_view::npos && line_start < text.size()) {
    size_t line_end = text.find_first_of('\n', line_start);
    size_t length;
    if (line_end == std::string_view::npos) {
      length = text.size() - line_start;
    } else {
      length = line_end - line_start;
    }
    std::string_view line = text.substr(line_start, length);

    // --- process line ---

    float line_length = scale * CalculateLineLength(line, text_slot) / 1024.0f;

    next_pos.x = position.x + line_length * new_line_offset_factor;

    // --- process char ---

    text_slot.SetExtraScale(line_length * 78.0f / (symbol_height * 1024.0f));
    text_slot.SetTranslate(next_pos);
    text_slot.RenderPicking();

    if (line_end == std::string_view::npos) {
      break;
    }
    line_start = line_end + 1;
    next_pos.y -= symbol_height * ui_scale * 1024.0f / (78.0f * 8 / 12);
  }
  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderTextPicking(
    UiDynamicSprite& text_slot,
    const FixedSizeQueue<char, 64>* text,
    float scale, glm::vec2 position, Alignment alignment) {
  RenderTextPicking(text_slot, std::string_view(text->cbegin(), text->cend()),
                    scale, position, alignment);
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
    coords.push_back(RenderPhrase(data::gText[i]));
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// only 1-row text by now
TextRenderer::Aabb TextRenderer::RenderPhrase(std::string_view text) {
  int calculated_width = CalculateLineLength(text, *text_slot_);
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

void TextRenderer::AppendChar(int code) {
  if (!input_source_) {
    std::cerr << "no source for input" << std::endl;
    return;
  }
  if (!input_source_->SafePushBack(static_cast<char>(code))) {
    std::cerr << "input overflow: 64 max" << std::endl;
  }
}

void TextRenderer::RemoveLastChar() {
  input_source_->SafePopBack();
}

void TextRenderer::StartInput(
    UiDynamicSprite& text_slot,
    FixedSizeQueue<char, 64>* input_source) {
  BindCallbacks();
  input_source_ = input_source;
}

void TextRenderer::StopInput() {
  input_source_ = nullptr;
}

void TextRenderer::BindCallbacks() {
  glfwSetCharCallback(gWindow, TextRenderer::CharCallback);
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetMouseButtonCallback(gWindow, TextRenderer::MouseButtonCallback);
}

void TextRenderer::CharCallback(GLFWwindow* window, unsigned int codepoint) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  /// range of printable char
  if (codepoint > 31 && codepoint < 127) {
    global_data->text_renderer->AppendChar(codepoint);
  }
}


void TextRenderer::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {}

void TextRenderer::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_RELEASE) {
    return;
  }
  /// so either GLFW_PRESS or GLFW_REPEAT
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  if (key == GLFW_KEY_ESCAPE
      || key == GLFW_KEY_ENTER) {
    glfwSetCharCallback(gWindow, nullptr);
    (*global_data->cur_mode)->BindCallbacks();
  } else if (key == GLFW_KEY_BACKSPACE) {
    global_data->text_renderer->RemoveLastChar();
  }
}

void TextRenderer::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    glfwSetCharCallback(gWindow, nullptr);
    (*global_data->cur_mode)->BindCallbacks();
  }
}
