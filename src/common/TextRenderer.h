#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string_view>

#include "../core/Ui.h"
#include "../ui/UiRenderData.h"
#include "../render/Shader.h"
#include "Font.h"
#include "../render/Texture.h"

class UiTextInput;

/// usage of GL_A (no color & transparency - one channel serves for both)

class TextRenderer {
 public:
  struct Aabb {
    int left;
    int right;
    int top;
    int bottom;
  };
  enum class Alignment { kLeft, kRight, kCentre };

  TextRenderer(UiRenderData& render_data,
               UiSprite&& prerender_text_slot, UiSprite&& sprite_cursor);

  ~TextRenderer();

  void RenderTextSelected(UiSprite& text_slot, std::string_view text,
                          glm::vec2 translate);

  void RenderText(UiSprite& text_slot, std::string_view text,
                  float scale = 1.0f, glm::vec2 position = glm::vec2{0.0f},
                  Alignment alignment = Alignment::kLeft);

  /// ALL picking use default dynamic sprite picking, so we don't even set it

  void RenderTextPicking(UiSprite& text_slot, std::string_view text,
                         float scale = 1.0f,
                         glm::vec2 position = glm::vec2{0.0f},
                         Alignment alignment = Alignment::kLeft);

  void RenderMenuText(UiSprite& text_slot, data::TextId id);

  void RenderMenuTextPicking(UiSprite& text_slot);

  void RenderModeText(UiSprite& text_slot, data::TextId id);

  void RenderModeTextPicking(UiSprite& text_slot);

  /// all prerendered text located together in data::TextId
  void PrerenderMenuText(int start, int end);

  void PrerenderModeText(int start, int end);

  /// --- INPUT SECTION ---

  void StartInput(UiTextInput* input_data);

  void StopInput();

  [[nodiscard]] bool InputInProgress() const noexcept {
    return input_in_progress_;
  }

  void AppendChar(int code);

  void BtnBackspace();

  void BtnDelete();

  void RenderInput();

  void BindCallbacks();

 private:
  static void CharCallback(GLFWwindow* window, unsigned int codepoint);

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);

  void RenderMenuModeText(const Texture& tex_prerender,
                          const std::vector<TextRenderer::Aabb>& tex_coords,
                          UiSprite& text_slot, int id);

  void RenderMenuModeTextPicking(const Texture& tex_prerender,
                                 UiSprite& text_slot);

  static int GetWidth(int code);

  static Aabb GetGlyphCoords(char ch);

  glm::mat3 CoordsToTransformMatrix(TextRenderer::Aabb aabb);

  void Init();
  void DeInit();

  void SetupFramebuffer(GLuint fbo_id, Texture& texture, bool clear);

  void RenderSymbol(char ch);

  Aabb RenderPhrase(std::string_view);

  float CalculateLineLength(std::string_view text, const UiSprite& text_slot);

  void PrerenderImpl(int start, int end, Texture& texture,
                     std::vector<Aabb>& coords);

  int cur_mode_start_ = 0;
  int cur_mode_end_ = 0;

  // left-top of previous, so start from the top
  glm::ivec2 fbo_cursor_{0, font::gSize};
  GLuint fbo_read_id_;
  GLuint fbo_write_id_;

  Texture tex_menu_;
  Texture tex_mode_;
  Texture tex_bitmap_;

  std::vector<Aabb> coords_menu_;
  std::vector<Aabb> coords_mode_;

  Shader render_shader_;
  Shader render_shader_picking_;

  // where to read & write
  UiSprite* text_slot_ = nullptr;

  UiSprite prerender_text_slot_;

  UiSprite sprite_cursor_;

  int CursorFromMousePos();

  bool IsCursorOnInputLine();

  void CalculateCursorPos(UiSprite& text_slot, const std::string& text);

  void MoveCursor(int value);

  // Unicode Text Segmentation rules (UAX #29)?
  // of course not :D, check ' ' and '_' only
  int LeftCtrlDistance();

  int RightCtrlDistance();

  void RemoveSelection();

  void TextInsert(std::string& buffer, char ch, int pos);

  void TextErase(std::string& buffer, int pos_start, int pos_end);

  bool input_in_progress_ = false;
  UiTextInput* input_data_ = nullptr;
  std::string buffer_input_;  // input here, after input back to input_data_

  /// not string, still need calculate "non-selected offset"
  int selected_start_ = 0;
  int selected_end_ = 0;   // cursor_pos as well
  float show_offset_ = 0;  // glScissor for cursor_pos (clipping)

  bool smt_selected_ = true;
  bool mouse_selection_ = false;

  UiRenderData& render_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
