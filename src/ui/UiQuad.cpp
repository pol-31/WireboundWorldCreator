#include "UiQuad.h"

struct Vertices {
  UiVertex lb, rb, lt, rt;
  UiQuad GenQuad() { return {lb, rb, rt, lb, rt, lt}; }
};

static Vertices GenQuadVertices(glm::mat3 transform, glm::vec4 color) {
  Vertices v;
  v.lb.color = color;
  v.rb.color = color;
  v.lt.color = color;
  v.rt.color = color;
  v.lb.pos = transform * glm::vec3(-1.0f, -1.0f, 1.0f);
  v.rb.pos = transform * glm::vec3(1.0f, -1.0f, 1.0f);
  v.lt.pos = transform * glm::vec3(-1.0f, 1.0f, 1.0f);
  v.rt.pos = transform * glm::vec3(1.0f, 1.0f, 1.0f);
  return v;
}

// TODO: accumulated all parent, local, debug transform, screen resolution
UiQuad UiQuad::GenSprite(data::UiId tc_id, glm::mat3 transform,
                         glm::vec4 color) {
  auto v = GenQuadVertices(transform, color);
  auto tc_quad = data::GetTexCoords(tc_id);
  v.lb.tc = tc_quad.lb;
  v.rb.tc = tc_quad.rb;
  v.lt.tc = tc_quad.lt;
  v.rt.tc = tc_quad.rt;
  return v.GenQuad();
}

UiQuad UiQuad::GenText(glm::mat3 transform, glm::vec4 color) {
  auto v = GenQuadVertices(transform, color);
  return v.GenQuad();
}
