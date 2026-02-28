#include "UiRenderer.h"

#include <Jolt/Core/Profiler.h>

#include "../io/Window.h"
#include "../render/Font.h"
#include "UiElement.h"

const int UiRenderer::gMaxRenderQuads = 200;

UiRenderer::UiRenderer(TextRenderer& text_renderer)
    : text_renderer_(text_renderer),
      tex_ui_("../assets/UiSprites.png", Texture::Type::TerrainRGBA8),
      tex_ui_mask_("../assets/tex_ui_mask.png", Texture::Type::TerrainRGBA8),
      shader_sp_mask_("../shaders/Sprite.vert", "../shaders/SpriteMask.frag",
                      {0, 6}),
      shader_sp_("../shaders/Sprite.vert", "../shaders/Sprite.frag", {0}),
      shader_sp_picking_("../shaders/Sprite.vert",
                         "../shaders/SpritePicking.frag", {}),
      shader_sp_hmap_("../shaders/Sprite.vert", "../shaders/SpriteHmap.frag",
                      {0}) {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, gMaxRenderQuads * sizeof(UiQuad), nullptr,
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);  // vec2 position (normalized ndc)

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);  // vec2 texture coordinate

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(4 * sizeof(float)));
  glEnableVertexAttribArray(2);  // vec4 color
}

UiRenderer::~UiRenderer() {
  while (!layers_.empty()) PopLayer();

  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void UiRenderer::Collect(float delta_time) {
  sprites_.clear();
  text_.clear();
  std::vector<UiQuad> data;
  for (auto layer : layers_) {
    layer.element->Update(delta_time, layer);
    PrerenderText(layer.buffer_text, layer.text);
    data.insert(data.end(), layer.buffer_sprites.begin(),
                layer.buffer_sprites.end());
    data.insert(data.end(), layer.buffer_text.begin(), layer.buffer_text.end());
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(UiQuad),
                  data.data());
}

int RenderLayer(int sprites_num, int text_num, int offset) {
  int size = sprites_num * 2;
  glDrawArrays(GL_TRIANGLES, offset, size);  // 2 triangles per quad
  offset += size;
  size = text_num * 2;
  glDrawArrays(GL_TRIANGLES, offset, size);  // 2 triangles per quad
  offset += size;
  return offset;
}

void UiRenderer::Render() const {
  glEnable(GL_SCISSOR_TEST);
  glBindVertexArray(vao_);
  shader_.Bind();
  tex_ui_.BindSampler(0);

  int offset = 0;
  if (is_draw_only_top_) {
    for (int i = 0; i < layers_.size() - 1; ++i) {
      offset += sprites_.size();
      offset += text_.size();
    }
    const auto& layer = layers_[0];
    glScissor(layer.scissors.x, layer.scissors.y, layer.scissors.width,
              layer.scissors.height);
    RenderLayer(sprites_.size(), text_.size(), offset * 2);
  } else {
    for (auto layer : layers_) {
      glScissor(layer.scissors.x, layer.scissors.y, layer.scissors.width,
                layer.scissors.height);
      offset = RenderLayer(sprites_.size(), text_.size(), offset);
    }
  }
  glDisable(GL_SCISSOR_TEST);
}

void UiRenderer::AddElement(UiElement* element) {
  if (layers_.empty()) {
    layers_.emplace_back();
  }
  layers_.back().element->Add(element);
}

void UiRenderer::PushLayer() { layers_.emplace_back(); }

void UiRenderer::PopLayer() {
  layers_.pop_back();  // TODO: delete it
}

UiElement* UiRenderer::FindByID(int id) {
  UiElement* element = nullptr;
  for (auto layer : layers_) {
    element = layer.element->FindByID(id);
  }
  return element;
}
