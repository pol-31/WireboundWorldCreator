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

WireboundWorldCreator::WireboundWorldCreator(const Paths& paths)
    : cursor_(),
      camera_(),
      interface_(paths, camera_, cursor_) {
  Init(paths);
}

void WireboundWorldCreator::RunRenderLoop() {
  camera_.UpdateProjectionMatrix();
  while (!glfwWindowShouldClose(gWindow)) {
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

  std::vector<std::string_view> cubemap_textures;
  cubemap_textures.reserve(6);
  cubemap_textures.push_back(paths.texture_skybox1_right);
  cubemap_textures.push_back(paths.texture_skybox1_left);
  cubemap_textures.push_back(paths.texture_skybox1_top);
  cubemap_textures.push_back(paths.texture_skybox1_bottom);
  cubemap_textures.push_back(paths.texture_skybox1_front);
  cubemap_textures.push_back(paths.texture_skybox1_back);
  cubemap_ = Cubemap(cubemap_textures, paths.shader_cubemap_vert,
                     paths.shader_cubemap_frag);
}
