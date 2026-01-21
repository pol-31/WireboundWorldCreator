#include "WireboundWorldCreator.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#include "GLFW/glfw3.h"
#include "TileRenderer.h"

WireboundWorldCreator::WireboundWorldCreator()
    : ui_renderer_(global_data_, tile_renderer_, &camera_) {
  Init();
}

void WireboundWorldCreator::RunRenderLoop() {
  CheckGlobalData();
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  while (!glfwWindowShouldClose(gWindow)) {
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto current_frame = static_cast<float>(glfwGetTime());
    gDeltaTime = current_frame - last_frame;
    last_frame = current_frame;
    tile_renderer_.cur_tile_.UpdateMapScale(gDeltaTime);
    cubemap_.Render();

    global_data_.UpdateCursorPos();
    global_data_.UpdateHoveredId();

    ui_renderer_.Render();

    picking_fbo_.Bind();
    ui_renderer_.RenderPicking();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void WireboundWorldCreator::Init() {
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(&global_data_));

  global_data_.camera = &camera_;
  global_data_.tile_renderer = &tile_renderer_;
  global_data_.picking_fbo = &picking_fbo_;

  ui_renderer_.SetupGlobalData();

  glEnable(GL_MULTISAMPLE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glClearColor(0.2f, 0.7f, 0.1f, 1.0f);

  ui_renderer_.Parse();

  global_data_.menu->SetMode(0);
}

void WireboundWorldCreator::DeInit() { /* ui_renderer_.Serialize(); */ }

void WireboundWorldCreator::CheckGlobalData() {
  if (!global_data_.camera || !global_data_.tile_renderer ||
      !global_data_.cur_mode || !global_data_.menu ||
      !global_data_.picking_fbo || !global_data_.ui_debugger ||
      !global_data_.ui_shared_resources || !global_data_.text_renderer ||
      !global_data_.windows) {
    throw "init global glfw callback data plz";
  }
}
