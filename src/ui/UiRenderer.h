#ifndef WIREBOUNDWORLDCREATOR_UI_UIRENDERER_H
#define WIREBOUNDWORLDCREATOR_UI_UIRENDERER_H

#include <memory>

#include "../common/TextRenderer.h"
#include "../render/Shader.h"
#include "UiQuad.h"

class Font;
class Renderer;

const float cActivateScreenTime = 0.2f;

class UiRenderer {
 public:
  UiRenderer(TextRenderer& text_renderer);

  ~UiRenderer();

  void Collect(float inDeltaTime);

  /// Draw elements
  void Render() const;

  // TODO: seems it should own it (so std::unique_ptr)
  void AddElement(UiElement* element);

  void PushLayer();

  void PopLayer();

  [[nodiscard]] int GetNumLayers() const {
    return static_cast<int>(layers_.size());
  }

  void SetIsDrawOnlyTop(bool is_draw_only_top) {
    is_draw_only_top_ = is_draw_only_top;
  }

  UiElement* FindByID(int inID);

  static const int gMaxRenderQuads;  // including sprites & text

 private:
  TextRenderer& text_renderer_;

  Shader shader_;  // TODO: external fbo; render to color & id textures
  GLuint vao_;
  GLuint vbo_;

  UiBuffer sprites_;
  UiBuffer text_;

  std::vector<UiLayer> layers_;

  bool is_draw_only_top_ = true;

  // --- prom prev UiRenderData
  Texture tex_ui_;
  Texture tex_ui_mask_;
  Shader shader_sp_;
  Shader shader_sp_mask_;
  Shader shader_sp_picking_;
  Shader shader_sp_hmap_;
};

#endif  // WIREBOUNDWORLDCREATOR_UI_UIRENDERER_H
