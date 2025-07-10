#include "WireboundWorldCreator.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "TileRenderer.h"

WireboundWorldCreator::WireboundWorldCreator(
    const Paths& paths)
    : tile_renderer_(paths),
      ui_renderer_(paths, global_data_, tile_renderer_) {
  Init(paths);
}

WireboundWorldCreator::~WireboundWorldCreator() {
  DeInit();
}

void WireboundWorldCreator::RunRenderLoop() {
  CheckGlobalData();
//  camera_.UpdateProjectionMatrix();
  while (!glfwWindowShouldClose(gWindow)) {
    /// internal fbos could modify it
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto current_frame = static_cast<float>(glfwGetTime());
    gDeltaTime = current_frame - last_frame;
    last_frame = current_frame;
    camera_.Update();
    cubemap_.Render();

    global_data_.UpdateCursorPos();

    auto pressed_id = global_data_.GetIdByMousePos();
    auto description_id = ui_renderer_.Hover(pressed_id);

    tile_renderer_.Render();
    ui_renderer_.Render(description_id);

    picking_fbo_.Bind();

    tile_renderer_.RenderPicking();
    ui_renderer_.RenderPicking(description_id);

    glfwPollEvents();
    glfwSwapBuffers(gWindow);

    /// unbind picking fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
}

void WireboundWorldCreator::Init(const Paths& paths) {
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(&global_data_));

  global_data_.camera = &camera_;
  global_data_.tile_renderer = &tile_renderer_;
  global_data_.picking_fbo = &picking_fbo_;

  ui_renderer_.SetupGlobalData();

  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
//  glDepthFunc(GL_LESS);
  glDepthFunc(GL_LEQUAL);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glClearColor(0.2f, 0.7f, 0.1f, 1.0f);

  std::array<std::string, 6> cubemap_textures{
    paths.texture_skybox1_right,
    paths.texture_skybox1_left,
    paths.texture_skybox1_top,
    paths.texture_skybox1_bottom,
    paths.texture_skybox1_front,
    paths.texture_skybox1_back
  };
  cubemap_ = Cubemap(cubemap_textures, paths.shader_cubemap_vert,
                     paths.shader_cubemap_frag);

  ui_renderer_.Parse();

  global_data_.menu->SetMode(0);
}

void WireboundWorldCreator::DeInit() {
  ui_renderer_.Serialize();
}

void WireboundWorldCreator::CheckGlobalData() {
  if (!global_data_.camera ||
      !global_data_.tile_renderer ||
      !global_data_.cur_mode ||
      !global_data_.menu ||
      !global_data_.picking_fbo ||
      !global_data_.ui_debugger ||
      !global_data_.ui_shared_resources ||
      !global_data_.text_renderer ||
      !global_data_.windows) {
    throw "init global glfw callback data plz";
  }
}
