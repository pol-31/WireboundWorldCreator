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

  TextRenderer(UiDynamicSprite&& text_slot, const Paths& paths);

  ~TextRenderer();

  void AppendChar(int code);

  void RemoveLastChar();

  /// binds its callbacks
  void StartInput(FixedSizeQueue<char, 64>* input_source);

  void StopInput();

  /// runtime text, no prerender:  each symbol rendered as a separate sprite
  /// (todo; prerender to some point is still possible)
  void RenderText(UiDynamicSprite& text_slot, std::string_view text,
                  float scale, glm::vec2 position,
                  Alignment alignment = Alignment::kCentre);

  void RenderText(UiDynamicSprite& text_slot,
                  const FixedSizeQueue<char, 64>* text,
                  float scale, glm::vec2 position,
                  Alignment alignment = Alignment::kCentre);

  void RenderTextPicking(UiDynamicSprite& text_slot, std::string_view text,
                         float scale, glm::vec2 position,
                         Alignment alignment = Alignment::kCentre);

  void RenderTextPicking(UiDynamicSprite& text_slot,
                         const FixedSizeQueue<char, 64>* text,
                         float scale, glm::vec2 position,
                         Alignment alignment = Alignment::kCentre);

  void RenderMenuText(data::TextId id);

  void RenderMenuTextPicking(data::TextId id);

  void RenderModeText(data::TextId id);

  void RenderModeTextPicking(data::TextId id);

  void PrerenderMenuText(int start, int end);

  void PrerenderModeText(int start, int end);

 private:
  void BindCallbacks();

  static void CharCallback(GLFWwindow* window, unsigned int codepoint);

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

  const float scale_{1.0f};

  Shader render_shader_;
  Shader render_shader_picking_;

  UiDynamicSprite text_slot_;

  FixedSizeQueue<char, 64>* input_source_{nullptr};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
