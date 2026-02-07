#include "TextRenderer.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <stb_image_write.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "../common/Font.h"
#include "../common/Text.h"
#include "../core/UiText.h"
#include "../io/Window.h"
#include "../modes/IUiMode.h"
#include "UiDebugger.h"

// TODO: we do factor 8:
//   next_pos.x += symbol_width / 8.0f;
//  due to sprite tex coords in Vbos.cpp (auto-generated)

TextRenderer::TextRenderer(UiRenderData& render_data,
                           UiSprite&& prerender_text_slot,
                           UiSprite&& sprite_cursor)
    : tex_bitmap_("../assets/tex_ascii.png", GL_RED, GL_RED, GL_UNSIGNED_BYTE),
      prerender_text_slot_(std::move(prerender_text_slot)),
      sprite_cursor_(std::move(sprite_cursor)),
      tex_menu_(1024, 1024, GL_RED, GL_RED, GL_UNSIGNED_BYTE),
      tex_mode_(1024, 1024, GL_RED, GL_RED, GL_UNSIGNED_BYTE),
      render_shader_("../shaders/Text.vert", "../shaders/Text.frag", {0}),
      render_shader_picking_("../shaders/Sprite.vert",
                             "../shaders/SpritePicking.frag", {}),
      render_data_(render_data) {
  Init();
}

TextRenderer::~TextRenderer() { DeInit(); }

void TextRenderer::Init() {
  render_shader_.Bind();
  glUniform1f(2, 1.0f);
  glUniform1f(1, 1.0f);
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

void TextRenderer::RenderTextSelected(UiSprite& text_slot,
                                      std::string_view text,
                                      glm::vec2 translate) {
  float symbol_height = font::gFullHeight / 1024.0f;
  tex_bitmap_.BindSampler(0);
  render_shader_.Bind();
  float ui_scale =
      debug::gUiTransforms[4 * (text_slot.GetId() - details::kIdOffsetUi)]
          .scale;
  glm::vec2 next_pos = glm::vec2{0.0f, symbol_height};

  int start = std::min(selected_start_, selected_end_);
  int end = std::max(selected_start_, selected_end_);

  for (int i = 0; i < start; ++i) {
    char ch = text[i];
    // TODO: if dbg
    if (ch == '\n') {
      throw "RenderTextSelected: remove \\n";
    }
    /// skip first
    next_pos.x += static_cast<float>(GetWidth(static_cast<int>(ch) - 32));
  }
  next_pos.x *= ui_scale / 78.0f;
  next_pos += translate - glm::vec2{input_data_->extra_width_, 0.0f};

  for (int i = start; i < end; ++i) {
    char ch = text[i];
    // TODO: if dbg
    if (ch == '\n') {
      throw "RenderTextSelected: remove \\n";
    }

    auto coords = GetGlyphCoords(ch);
    auto coords_transform = CoordsToTransformMatrix(coords);
    float symbol_width =
        static_cast<float>(GetWidth(static_cast<int>(ch) - 32)) / 1024.0f;

    glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));

    text_slot.SetExtraScale(symbol_width / symbol_height);
    symbol_width *= ui_scale * 1024.0f / 78.0f;

    /// correct alignment (half prev, half next char)
    next_pos.x += symbol_width / 2.0f;
    text_slot.SetTranslate(next_pos);
    next_pos.x += symbol_width / 2.0f;
    text_slot.Render();
  }

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(UiSprite& text_slot, std::string_view text,
                              float scale, glm::vec2 position,
                              Alignment alignment) {
  float symbol_height = font::gFullHeight * scale / 1024.0f;
  tex_bitmap_.BindSampler(0);
  render_shader_.Bind();
  text_slot.SetScale(scale);
  float ui_scale =
      debug::gUiTransforms[4 * (text_slot.GetId() - details::kIdOffsetUi)]
          .scale;

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
    float line_length =
        (scale * CalculateLineLength(line, text_slot) / 1024.0f) / 4.0f;
    next_pos.x = position.x + line_length * new_line_offset_factor;

    // --- process char ---
    for (auto ch : line) {
      auto coords = GetGlyphCoords(ch);
      auto coords_transform = CoordsToTransformMatrix(coords);
      float symbol_width =
          scale * static_cast<float>(GetWidth(static_cast<int>(ch) - 32)) /
          1024.0f;

      glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));

      text_slot.SetExtraScale(symbol_width / symbol_height);
      symbol_width *= ui_scale * 1024.0f / 78.0f;

      /// correct alignment (half prev, half next char)
      next_pos.x += symbol_width / 8.0f;
      text_slot.SetTranslate(next_pos);
      text_slot.Render();
      next_pos.x += symbol_width / 8.0f;
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

void TextRenderer::RenderTextPicking(UiSprite& text_slot, std::string_view text,
                                     float scale, glm::vec2 position,
                                     Alignment alignment) {
  // picking for the first line is enough
  float symbol_height = font::gFullHeight * scale / 1024.0f;
  text_slot.SetScale(scale);
  float ui_scale =
      debug::gUiTransforms[4 * (text_slot.GetId() - details::kIdOffsetUi)]
          .scale;

  size_t line_start = 0;
  glm::vec2 next_pos{0.0f, position.y + symbol_height};

  float new_line_offset_factor;  // why another offset
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

    float line_length =
        (scale * CalculateLineLength(line, text_slot) / 1024.0f);
    next_pos.x = position.x + line_length * new_line_offset_factor / 4.0f;

    // --- process char ---
    text_slot.SetExtraScale((line_length * 78.0f / ui_scale) /
                            (symbol_height * 1024.0f));
    next_pos.x += line_length / 8.0f;
    text_slot.SetTranslate(next_pos);
    text_slot.RenderPicking();
    break;

    if (line_end == std::string_view::npos) {
      break;
    }
    line_start = line_end + 1;
    next_pos.y -= symbol_height * ui_scale * 1024.0f / (78.0f * 8 / 12);
  }
}

void TextRenderer::RenderMenuText(UiSprite& text_slot, data::TextId id) {
  RenderMenuModeText(tex_menu_, coords_menu_, text_slot, static_cast<int>(id));
}

void TextRenderer::RenderModeText(UiSprite& text_slot, data::TextId id) {
  RenderMenuModeText(tex_mode_, coords_mode_, text_slot,
                     static_cast<int>(id) - cur_mode_start_);
}

void TextRenderer::RenderMenuModeText(
    const Texture& tex_prerender,
    const std::vector<TextRenderer::Aabb>& tex_coords, UiSprite& text_slot,
    int id) {
  text_slot_ = &text_slot;
  tex_prerender.BindSampler(0);
  render_shader_.Bind();
  glUniform3f(4, 0.2118f, 0.1647f, 0.0745f);
  // TODO: dbg at() <- error possible if menu_id <-> mode_id
  Aabb coords = tex_coords.at(static_cast<int>(id));
  auto coords_transform = CoordsToTransformMatrix(coords);
  glUniformMatrix3fv(6, 1, false, glm::value_ptr(coords_transform));
  int width = coords.right - coords.left;
  int height = coords.top - coords.bottom;
  text_slot_->SetExtraScale(static_cast<float>(width) / height);
  text_slot_->Render();

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderModeTextPicking(UiSprite& text_slot) {
  RenderMenuModeTextPicking(tex_mode_, text_slot);
}

void TextRenderer::RenderMenuTextPicking(UiSprite& text_slot) {
  RenderMenuModeTextPicking(tex_menu_, text_slot);
}

void TextRenderer::RenderMenuModeTextPicking(const Texture& tex_prerender,
                                             UiSprite& text_slot) {
  text_slot_ = &text_slot;
  text_slot_->RenderPicking();
}

void TextRenderer::PrerenderMenuText(int start, int end) {
  text_slot_ = &prerender_text_slot_;
  PrerenderImpl(start, end, tex_menu_, coords_menu_);
}

void TextRenderer::PrerenderModeText(int start, int end) {
  cur_mode_start_ = start;
  cur_mode_end_ = end;
  text_slot_ = &prerender_text_slot_;
  PrerenderImpl(start, end, tex_mode_, coords_mode_);
}

void TextRenderer::PrerenderImpl(int start, int end, Texture& texture,
                                 std::vector<Aabb>& coords) {
  SetupFramebuffer(fbo_write_id_, texture, true);
  fbo_cursor_ = glm::ivec2{0, 1024};

  int size = end - start;
  coords.clear();
  coords.reserve(size);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_read_id_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_write_id_);
  for (int i = start; i < end; ++i) {
    coords.push_back(RenderPhrase(data::gText[i]));
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // texture.StoreImage("prerenderedTexture.png", 1);
}

TextRenderer::Aabb TextRenderer::RenderPhrase(std::string_view text) {
  int calculated_width =
      CalculateLineLength(text, *text_slot_) * 78.0f / 1024.0f;
  int symbol_height = static_cast<int>(font::gFullHeight);
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
  Aabb dst = {
      fbo_cursor_.x,
      fbo_cursor_.x + static_cast<int>(GetWidth(static_cast<int>(ch) - 32)),
      fbo_cursor_.y, fbo_cursor_.y - static_cast<int>(font::gFullHeight)};
  fbo_cursor_.x += static_cast<int>(GetWidth(static_cast<int>(ch) - 32));
  glBlitFramebuffer(src.left, src.bottom, src.right, src.top, dst.left,
                    dst.bottom, dst.right, dst.top, GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
}

TextRenderer::Aabb TextRenderer::GetGlyphCoords(char ch) {
  int id = static_cast<int>(ch);
  int column = id & 7;
  int local_id = id - 32;  // bitmap starts from 32
  int row = 12 - (static_cast<int>(local_id / 8));
  return {128 * column, 128 * column + GetWidth(local_id), 85 * (row),
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

void TextRenderer::RemoveSelection() {
  int selected_left = std::min(selected_start_, selected_end_);
  selected_end_ = selected_left;
  selected_start_ = selected_left;
  smt_selected_ = false;
}

void TextRenderer::StartInput(UiTextInput* input_data) {
  input_data_ = input_data;
  /// select all
  buffer_input_ = input_data->text_input_;
  selected_start_ = 0;
  selected_end_ = buffer_input_.size();
  smt_selected_ = true;
  BindCallbacks();
  input_in_progress_ = true;
}

void TextRenderer::StopInput() {
  glfwSetCharCallback(gWindow, nullptr);
  input_data_->SetText(buffer_input_);
  input_data_->SetTextTranslate(glm::vec2(0.0f));
  input_data_ = nullptr;
  (*render_data_.glfw_context_.cur_mode)->Setup();
  input_in_progress_ = false;
}

void TextRenderer::AppendChar(int code) {
  if (selected_start_ != selected_end_) {
    TextErase(buffer_input_, selected_start_, selected_end_);
  }
  RemoveSelection();
  TextInsert(buffer_input_, static_cast<char>(code), selected_end_);
  ++selected_end_;
  ++selected_start_;
}

void TextRenderer::BtnBackspace() {
  if (selected_start_ == selected_end_) {
    if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      selected_end_ = std::clamp(selected_end_ + LeftCtrlDistance(), 0,
                                 static_cast<int>(buffer_input_.size()));
    } else {
      selected_start_ = std::max(selected_end_ - 1, 0);
    }
  }
  TextErase(buffer_input_, selected_start_, selected_end_);
  RemoveSelection();
}

void TextRenderer::BtnDelete() {
  if (selected_start_ == selected_end_) {
    if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      selected_end_ = std::clamp(selected_end_ + RightCtrlDistance(), 0,
                                 static_cast<int>(buffer_input_.size()));
    } else {
      selected_end_ =
          std::min(selected_end_ + 1, static_cast<int>(buffer_input_.size()));
    }
  }
  TextErase(buffer_input_, selected_start_, selected_end_);
  RemoveSelection();
}

void TextRenderer::RenderInput() {
  if (mouse_selection_) {
    selected_end_ = CursorFromMousePos();
    smt_selected_ = true;
  }
  /// blur background, render back
  render_data_.tex_ui_.BindSampler(0);
  render_data_.shader_sp_.Bind();
  glUniform1f(1, 1.0f);
  input_data_->sp_back_.Render();

  /// render text & selected text
  tex_bitmap_.BindSampler(0);
  render_shader_.Bind();
  input_data_->SetText(buffer_input_);

  glEnable(GL_SCISSOR_TEST);
  input_data_->SetScissorArea();
  //  glScissor(x_start, 0, x_length, 4000);
  if (smt_selected_ || mouse_selection_) {
    // brightness location is 2
    glUniform1f(1, 0.4f);
    input_data_->RenderTextNoScissors();
    render_shader_.Bind();
    glUniform1f(1, 1.0f);
    RenderTextSelected(input_data_->sp_text_, input_data_->text_input_,
                       input_data_->GetTextTranslate());
  } else {
    glUniform1f(2, 1.0f);
    input_data_->RenderTextNoScissors();
  }
  glDisable(GL_SCISSOR_TEST);

  /// render cursor
  render_data_.tex_ui_.BindSampler(0);
  render_data_.shader_sp_.Bind();
  CalculateCursorPos(input_data_->sp_text_, input_data_->text_input_);
  sprite_cursor_.Render();  // has it's position
}

int TextRenderer::CursorFromMousePos() {
  float ui_scale = debug::gUiTransforms[4 * (input_data_->sp_text_.GetId() -
                                             details::kIdOffsetUi)]
                       .scale;
  auto mouse_pos_x = render_data_.glfw_context_.cursor_pos_tex_norm_.x;
  float next_pos = input_data_->GetLeftBorder();
  int i = 0;
  for (; i < buffer_input_.size(); ++i) {
    char ch = input_data_->text_input_[i];
    // TODO: if dbg
    if (ch == '\n') {
      throw "RenderTextSelected: remove \\n";
    }
    /// skip first
    auto char_length = static_cast<float>(GetWidth(static_cast<int>(ch) - 32)) *
                       ui_scale / 78.0f;
    if (mouse_pos_x < next_pos + char_length) {
      break;
    }
    next_pos += char_length;
  }
  return i;
}

void TextRenderer::CalculateCursorPos(UiSprite& text_slot,
                                      const std::string& text) {
  float symbol_height = font::gFullHeight / 1024.0f;
  float ui_scale =
      debug::gUiTransforms[4 * (text_slot.GetId() - details::kIdOffsetUi)]
          .scale;
  glm::vec2 next_pos{0.0f, symbol_height};
  for (int i = 0; i < selected_end_; ++i) {
    char ch = text[i];
    // TODO: if dbg
    if (ch == '\n') {
      throw "RenderTextSelected: remove \\n";
    }
    /// skip first
    next_pos.x += static_cast<float>(GetWidth(static_cast<int>(ch) - 32));
  }
  next_pos.x *= ui_scale / 78.0f;
  auto left_slot_border = input_data_->GetLeftBorder();
  auto right_slot_border = input_data_->GetRightBorder();
  /// remove prev translate, set new
  float next_pos_x_rel = next_pos.x + left_slot_border;
  input_data_->SetTextTranslate(glm::vec2(0.0f));
  float bias = 0.005f;
  if (next_pos_x_rel + show_offset_ < left_slot_border) {
    show_offset_ = left_slot_border - next_pos_x_rel + bias;
  } else if (next_pos_x_rel + show_offset_ > right_slot_border) {
    show_offset_ = right_slot_border - next_pos_x_rel - bias;
  }
  input_data_->SetTextTranslate(glm::vec2(show_offset_, 0.0f));
  sprite_cursor_.SetTranslate(next_pos);
}

// smt already selected -> shift ? move cursor : deselect & move cursor &
// start=end shift ? move cursor & smt_selected=true : move cursor & start & end
void TextRenderer::MoveCursor(int value) {
  smt_selected_ = glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
  if (smt_selected_) {
    selected_end_ = std::clamp(selected_end_ + value, 0,
                               static_cast<int>(buffer_input_.size()));
    return;
  }
  if (selected_start_ == selected_end_) {
    selected_end_ = std::clamp(selected_end_ + value, 0,
                               static_cast<int>(buffer_input_.size()));
  } else {
    // deselect & move to selection border
    int set_value;
    if (value < 0) {
      set_value = std::min(selected_start_, selected_end_);
    } else {
      set_value = std::max(selected_start_, selected_end_);
    }
    selected_end_ = set_value;
  }
  selected_start_ = selected_end_;
}

int TextRenderer::LeftCtrlDistance() {
  int dist = 0;
  for (int i = selected_end_ - 1; i >= 0; --i) {
    auto code = static_cast<int>(buffer_input_[i]);
    if (code == static_cast<int>(' ') || code == static_cast<int>('_')) {
      if (i != selected_end_ - 1) {
        break;  // skip if it's the first char
      }
    }
    --dist;
  }
  return dist;
}

int TextRenderer::RightCtrlDistance() {
  int dist = 0;
  for (int i = selected_end_ + 1; i <= buffer_input_.size(); ++i) {
    auto code = static_cast<int>(buffer_input_[i - 1]);
    if (code == static_cast<int>(' ') || code == static_cast<int>('_')) {
      if (i != selected_end_ + 1) {
        break;  // skip if it's the first char
      }
    }
    ++dist;
  }
  return dist;
}

void TextRenderer::BindCallbacks() {
  glfwSetCharCallback(gWindow, TextRenderer::CharCallback);
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetKeyCallback(gWindow, TextRenderer::KeyCallback);
  glfwSetMouseButtonCallback(gWindow, TextRenderer::MouseButtonCallback);
}

void TextRenderer::SetupFramebuffer(GLuint fbo_id, Texture& texture,
                                    bool clear) {
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

int TextRenderer::GetWidth(int code) {
  float comp_width;
  if (code & 1) {
    comp_width = static_cast<float>((font::gWidths[code >> 1]) >> 4);
  } else {
    comp_width = static_cast<float>((font::gWidths[code >> 1]) & 0x0F);
  }
  auto result =
      static_cast<int>(comp_width * font::gWidthFactor) + font::gWidthMin;
  return result;
}

float TextRenderer::CalculateLineLength(std::string_view text,
                                        const UiSprite& text_slot) {
  int total_length = 0;
  for (auto ch : text) {
    if (ch == '\n') {
      break;
    }
    total_length += GetWidth(static_cast<int>(ch) - 32);
  }
  float ui_scale =
      debug::gUiTransforms[4 * (text_slot.GetId() - details::kIdOffsetUi)]
          .scale;
  return static_cast<float>(total_length) * ui_scale * 1024.0f / 78.0f;
}

bool TextRenderer::IsCursorOnInputLine() {
  auto cursor_pos_y = render_data_.glfw_context_.cursor_pos_tex_norm_.y;
  return cursor_pos_y < input_data_->sp_back_.GetTopBorder() &&
         cursor_pos_y > input_data_->sp_back_.GetBottomBorder();
}

void TextRenderer::TextInsert(std::string& buffer, char ch, int pos) {
  buffer.insert(buffer.begin() + pos, ch);
}

void TextRenderer::TextErase(std::string& buffer, int pos_start, int pos_end) {
  if (pos_start < pos_end) {
    buffer.erase(buffer.begin() + pos_start, buffer.begin() + pos_end);
  } else {
    buffer.erase(buffer.begin() + pos_end, buffer.begin() + pos_start);
  }
}

void TextRenderer::CharCallback(GLFWwindow* window, unsigned int codepoint) {
  auto glfw_context = GetGlfwContext(window);
  /// range of printable char
  if (codepoint > 31 && codepoint < 127) {
    glfw_context->text_renderer->AppendChar(codepoint);
  }
}

void TextRenderer::KeyCallback(GLFWwindow* window, int key, int scancode,
                               int action, int mods) {
  if (action == GLFW_RELEASE) {
    return;
  }
  /// so either GLFW_PRESS or GLFW_REPEAT
  auto glfw_context = GetGlfwContext(window);
  glfw_context->text_renderer->mouse_selection_ = false;
  if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_ENTER) {
    glfw_context->text_renderer->StopInput();
  } else if (key == GLFW_KEY_BACKSPACE) {
    glfw_context->text_renderer->BtnBackspace();
  } else if (key == GLFW_KEY_DELETE) {
    glfw_context->text_renderer->BtnDelete();
  } else if (key == GLFW_KEY_LEFT) {
    int move_value = -1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      move_value = glfw_context->text_renderer->LeftCtrlDistance();
    }
    glfw_context->text_renderer->MoveCursor(move_value);
  } else if (key == GLFW_KEY_RIGHT) {
    int move_value = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      move_value = glfw_context->text_renderer->RightCtrlDistance();
    }
    glfw_context->text_renderer->MoveCursor(move_value);
  }
}

void TextRenderer::MouseButtonCallback(GLFWwindow* window, int button,
                                       int action, int mods) {
  if (button != GLFW_MOUSE_BUTTON_LEFT) {
    return;
  }
  auto glfw_context = GetGlfwContext(window);
  if (action == GLFW_PRESS) {
    if (glfw_context->text_renderer->IsCursorOnInputLine()) {
      glfw_context->text_renderer->mouse_selection_ = true;
      glfw_context->text_renderer->selected_start_ =
          glfw_context->text_renderer->CursorFromMousePos();
    } else {
      glfw_context->text_renderer->StopInput();
    }
  } else {  // GLFW_RELEASE
    glfw_context->text_renderer->mouse_selection_ = false;
  }
}
