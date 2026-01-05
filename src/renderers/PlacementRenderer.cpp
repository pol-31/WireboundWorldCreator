#include "PlacementRenderer.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../common/OpenGLUtility.h"
#include "../io/Window.h"

void PlacementRenderer::Render() {
  grass_.UpdateAnimation(gDeltaTime);
  if (glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // grass_.Render();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PlacementRenderer::RenderPicking() {}

float PlacementRenderer::PlacementHash(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352d;
  x ^= x >> 15;
  x *= 0x846ca68b;
  x ^= x >> 16;
  return float(x) * (1.0f / 4294967296.0f);
}

std::vector<GLuint> PlacementRenderer::UpdatePipeline(
    Texture& placement, float density, uint32_t seed) {
  density = std::clamp(density, 0.0f, 1.0f);

  std::vector<GLuint> result;
  const int size = 1024 * 1024;

  std::vector<uint8_t> data(size);
  glGetTextureImage(
      placement.GetId(), 0,
      GL_RED, GL_UNSIGNED_BYTE,
      size, data.data()
  );

  for (uint32_t i = 0; i < size; ++i) {
    if (!data[i]) {
      continue;
    }
    seed = i ^ (seed * 0x9E3779B9u);
    if (PlacementHash(seed) < density) {
      result.push_back(i);
    }
  }

  return result;
}
