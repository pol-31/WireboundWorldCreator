#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_

#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"
#include "Paths.h"
#include "../core/Ui.h"


void TextRendererCharCallback(GLFWwindow* window, unsigned int codepoint);

void TextRendererMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

/// usage of GL_A (no color & transparency - one channel serves for both)

class TextRenderer {
 public:
  struct Aabb {
    int left;
    int right;
    int top;
    int bottom;
  };

  void AppendChar(int code) {
    if (!input_source_) {
      std::cerr << "no source for input" << std::endl;
      return;
    }
    input_source_->PushBack(static_cast<char>(code));
  }

  void StartInput(FixedSizeQueue<char, 64>* input_source) {
    input_source_ = input_source;
  }

  void StopInput() {
    input_source_ = nullptr;
  }

  TextRenderer(UiDynamicSprite&& text_slot, const Paths& paths);

  ~TextRenderer();

  /// runtime text, no prerender:  each symbol rendered as a separate sprite
  /// (todo; prerender to some point is still possible)
  void RenderText(/*UiDynamicSprite& text_slot, */std::string_view text,
                  float scale, glm::vec2 position);

  void RenderText(const FixedSizeQueue<char, 64>* text,
                  float scale, glm::vec2 position);

  void RenderTextPicking(/*UiDynamicSprite& text_slot, */std::string_view text,
                         float scale, glm::vec2 position);

  void RenderTextPicking(const FixedSizeQueue<char, 64>* text,
                         float scale, glm::vec2 position);

  void RenderMenuText(int id);

  void RenderMenuTextPicking(int id);

  void RenderModeText(int id);

  void RenderModeTextPicking(int id);

  void PrerenderMenuText(int start, int end);

  void PrerenderModeText(int start, int end);

 private:
  static Aabb GetGlyphCoords(char ch);

  glm::mat3 CoordsToTransformMatrix(TextRenderer::Aabb aabb);

  void Init();
  void DeInit();

  void SetupFramebuffer(GLuint fbo_id, Texture& texture, bool clear);

  void RenderSymbol(char ch);

  Aabb RenderPhrase(std::string_view);

  int CalculateLength(std::string_view text);

  void PrerenderImpl(
      int start, int end, Texture& texture,
      std::vector<Aabb>& coords);

  // vao, vbo (special slot per text pos), shader, the same, texture differs


  /// we don't need invalidation ids - rerender only *good amount of text,
  /// so it's 100% pre-rendered, so don't need active_ or even id_-s

  // left-top of previous, so start from the top
  glm::ivec2 fbo_cursor_{0, 1024};
  //  int fbo_cursor_lowest_{1920}; // don't need if scale is the same
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
