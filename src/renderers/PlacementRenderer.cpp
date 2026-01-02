#include "PlacementRenderer.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../common/OpenGLUtility.h"
#include "../io/Window.h"

PlacementRenderer::PlacementRenderer()
    : poisson_shader_("../shaders/PoissonPoints.comp"),
      density_low_("../assets/poisson/500.png", GL_RGBA8),
      density_medium_low_("../assets/poisson/1000.png", GL_RGBA8),
      density_medium_("../assets/poisson/2000.png", GL_RGBA8),
      density_medium_high_("../assets/poisson/5000.png", GL_RGBA8),
      density_high_("../assets/poisson/10000.png", GL_RGBA8),
      density_very_high_("../assets/poisson/20000.png", GL_RGBA8),
      density_ultra_high_("../assets/poisson/40000.png", GL_RGBA8),
      density_extreme_("../assets/poisson/80000.png", GL_RGBA8) {
  Init();
}

void PlacementRenderer::Init() {
  placement_temp_ = Texture(1024, 1024, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
  glClearTexImage(placement_temp_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);
}

void PlacementRenderer::Render() {
  grass_.UpdateAnimation(gDeltaTime);
  if (glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // grass_.Render();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PlacementRenderer::RenderPicking() {}

std::vector<GLuint> PlacementRenderer::UpdatePipeline(Texture& placement,
                                                      int density_level) {
  poisson_shader_.Bind();
  Texture* tex_density = nullptr;
  switch (density_level) {
    case 0:
      tex_density = &density_medium_;
      break;
    case 1:
      tex_density = &density_medium_high_;
      break;
    case 2:
      tex_density = &density_high_;
      break;
    default:
      tex_density = &density_extreme_;
      break;
  }
  utility::BindImageTexture(0, *tex_density, GL_READ_ONLY);
  utility::BindImageTexture(1, placement, GL_READ_ONLY);
  utility::BindImageTexture(2, placement_temp_, GL_WRITE_ONLY);
  glDispatchCompute(1024 / 16, 1024 / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  utility::UnBindImageTexture(0, *tex_density, GL_READ_ONLY);
  utility::UnBindImageTexture(1, placement, GL_READ_ONLY);
  utility::UnBindImageTexture(2, placement_temp_, GL_WRITE_ONLY);

  std::vector<unsigned char> data(1024 * 1024);
  placement_temp_.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());

  std::vector<GLuint> positions;
  for (int i = 0; i < 1024 * 1024; ++i) {
    if (data[i] != 0) {
      positions.push_back(i);
    }
  }
  std::cout << "Placement generated: " << positions.size() << std::endl;
  return positions;
}
