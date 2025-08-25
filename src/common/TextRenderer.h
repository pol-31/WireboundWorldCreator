#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_

#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"
#include "Paths.h"
#include "../core/Ui.h"
#include "Font.h"
#include "../core/UiText.h"

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
  enum class Alignment {
    kLeft,
    kRight,
    kCentre
  };

  TextRenderer(
      UiSharedResources& ui_shared_resources,
      const Paths& paths,
      UiDynamicSprite&& prerender_text_slot,
      UiDynamicSprite&& sprite_shadow,
      UiDynamicSprite&& sprite_cursor);

  ~TextRenderer();

  void AppendChar(int code);

  void BtnBackspace();

  /// binds its callbacks
  void StartInput(UiTextInput* input_data);

  void StopInput();

  void RenderInput();

  void RenderText(UiDynamicSprite& text_slot,
                  FixedSizeQueue<char, 64>* text);

  void RenderTextSelected(UiDynamicSprite& text_slot,
                          FixedSizeQueue<char, 64>* text);

  /// no RenderPickingTextSelected - we don't "pick" it
  void RenderPickingText(UiDynamicSprite& text_slot,
                         FixedSizeQueue<char, 64>* text);

  /// runtime text, no prerender:  each symbol rendered as a separate sprite
  /// (todo; prerender to some point is still possible)
  void RenderText(UiDynamicSprite& text_slot, std::string_view text,
                  float scale, glm::vec2 position,
                  Alignment alignment = Alignment::kCentre);

  void RenderText(UiDynamicSprite& text_slot, std::string_view text,
                  Alignment alignment = Alignment::kCentre) {
    RenderText(text_slot, text, 1.0f, glm::vec2{0.0f}, alignment);
  }

  void RenderText(UiDynamicSprite& text_slot,
                  const FixedSizeQueue<char, 64>* text,
                  float scale, glm::vec2 position,
                  Alignment alignment = Alignment::kCentre);

  void RenderTextPicking(UiDynamicSprite& text_slot, std::string_view text,
                         float scale, glm::vec2 position,
                         Alignment alignment = Alignment::kCentre);

  void RenderTextPicking(UiDynamicSprite& text_slot, std::string_view text,
                         Alignment alignment = Alignment::kCentre) {
    RenderTextPicking(text_slot, text, 1.0f, glm::vec2{0.0f}, alignment);
  }

  void RenderTextPicking(UiDynamicSprite& text_slot,
                         const FixedSizeQueue<char, 64>* text,
                         float scale, glm::vec2 position,
                         Alignment alignment = Alignment::kCentre);

  void RenderMenuText(
      UiDynamicSprite& text_slot, data::TextId id, float scale);

  void RenderMenuTextPicking(
      UiDynamicSprite& text_slot, data::TextId id);

  void RenderModeText(data::TextId id);

  void RenderModeTextPicking(data::TextId id);

  void PrerenderMenuText(int start, int end);

  void PrerenderModeText(int start, int end);

  void BindCallbacks();

  [[nodiscard]] bool InputInProgress() const noexcept {
    return input_in_progress_;
  }

 private:
  static void CharCallback(GLFWwindow* window, unsigned int codepoint);

  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static Aabb GetGlyphCoords(char ch);

  glm::mat3 CoordsToTransformMatrix(TextRenderer::Aabb aabb);

  void Init();
  void DeInit();

  void SetupFramebuffer(GLuint fbo_id, Texture& texture, bool clear);

  void RenderSymbol(char ch);

  Aabb RenderPhrase(std::string_view);

  int CalculateLineLength(
      std::string_view text, const UiDynamicSprite& text_slot);

  int CalculateLineLength(
      FixedSizeQueue<char, 64>* text, const UiDynamicSprite& text_slot);

  void PrerenderImpl(
      int start, int end, Texture& texture,
      std::vector<Aabb>& coords);

  /// we don't need invalidation ids - rerender only *good amount of text,
  /// so it's 100% pre-rendered, so don't need active_ or even id_-s

  // left-top of previous, so start from the top
  glm::ivec2 fbo_cursor_{0, font::gSize};
  GLuint fbo_read_id_;
  GLuint fbo_write_id_;

  Texture tex_menu_;
  Texture tex_mode_;
  Texture tex_bitmap_;

  std::vector<Aabb> coords_menu_;
  std::vector<Aabb> coords_mode_;

  const float scale_{0.1f};

  Shader render_shader_;
  Shader render_shader_picking_;

  // where to read & write
  UiDynamicSprite* text_slot_ = nullptr;
  FixedSizeQueue<char, 64>* input_source_ = nullptr;

  UiDynamicSprite prerender_text_slot_;

  UiDynamicSprite sprite_shadow_;
  UiDynamicSprite sprite_cursor_;


  bool input_in_progress_ = false;
  UiTextInput* input_data_ = nullptr;
  // input to buffer, only then copy to input_data_'s buffer
  FixedSizeQueue<char, 64> buffer_input_;

  /// not string, still need calculate "non-selected offset"
  int selected_start_ = 0;
  int selected_end_ = 0;
//  bool do_select_ = false; // same as selected_start != selected_end_
//  int cursor_pos_ = 0; // same as selected_end_

  static int GetWidth(int code);

  void CalculateCursorPos(
      UiDynamicSprite& text_slot, FixedSizeQueue<char, 64>* text);

  void MoveCursor(int value);

  int LeftCtrlDistance();

  int RightCtrlDistance();

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
