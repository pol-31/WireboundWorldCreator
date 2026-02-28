#ifndef WIREBOUNDWORLDCREATOR_UI_UITEXTUREDQUAD_H
#define WIREBOUNDWORLDCREATOR_UI_UITEXTUREDQUAD_H

#include <glm/glm.hpp>
#include <vector>

#include "../common/Vbos.h"

class UiElement;

struct UiVertex {
  glm::vec2 pos;  // TODO: vec3?
  glm::vec2 tc;   /// normalized
  glm::vec4 color;
};

/// at render both for sprites and text
struct UiQuad {
  UiVertex first_lb;
  UiVertex first_rb;
  UiVertex first_rt;
  UiVertex last_lb;
  UiVertex last_rt;
  UiVertex last_lt;

  static UiQuad GenSprite(data::UiId tc_id, glm::mat3 transform,
                          glm::vec4 color);

  // TODO: need 1 (ONE) prerender texture
  //  - yeah, let's have 1 texture for now
  static UiQuad GenText(glm::mat3 transform, glm::vec4 color);
};

using UiBuffer = std::vector<UiQuad>;
using UiBuffer = std::vector<UiQuad>;

struct UiScissorRect {
  int x = 0;
  int y = 0;
  int width = 2000;   // todo;
  int height = 2000;  // todo;
};

// have to separate text to 2 buffers to keep data together so push to vbo
struct UiLayer {
  UiElement* element;
  UiBuffer buffer_sprites;
  UiBuffer buffer_text;
  std::vector<std::string_view> text;
  UiScissorRect scissors;
};

#endif  // WIREBOUNDWORLDCREATOR_UI_UITEXTUREDQUAD_H
