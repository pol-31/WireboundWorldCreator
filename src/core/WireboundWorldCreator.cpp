#include "WireboundWorldCreator.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TileRenderer.h"
#include "../common/Shader.h"
#include "../common/Texture.h"

#include "../common/Details.h"
#include "../io/Window.h"
#include "../common/Vbos.h"
#include "Map.h"

WireboundWorldCreator::WireboundWorldCreator(const Paths& paths, Map& map)
    : cursor_(),
      camera_(),
      interface_(paths, camera_, cursor_, map) {
  Init(paths);
}

void WireboundWorldCreator::RunRenderLoop() {
  camera_.UpdateProjectionMatrix();
  while (!glfwWindowShouldClose(gWindow)) {
    // here, because framebuffers change it to black, so need to restore
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto current_frame = static_cast<float>(glfwGetTime());
    gDeltaTime = current_frame - last_frame;
    last_frame = current_frame;
    camera_.Update();
    cubemap_.Render();
    interface_.Render();

    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void WireboundWorldCreator::Init(const Paths& paths) {
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
//  glDepthFunc(GL_LESS);
  glDepthFunc(GL_LEQUAL);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glClearColor(0.2f, 0.7f, 0.1f, 1.0f);

  std::array<std::string, 6> cubemap_textures;
  cubemap_textures[0] = (paths.texture_skybox1_right);
  cubemap_textures[1] = (paths.texture_skybox1_left);
  cubemap_textures[2] = (paths.texture_skybox1_top);
  cubemap_textures[3] = (paths.texture_skybox1_bottom);
  cubemap_textures[4] = (paths.texture_skybox1_front);
  cubemap_textures[5] = (paths.texture_skybox1_back);
  cubemap_ = Cubemap(cubemap_textures, paths.shader_cubemap_vert,
                     paths.shader_cubemap_frag);
}
