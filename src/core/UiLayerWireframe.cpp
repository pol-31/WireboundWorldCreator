#include "UiLayerWireframe.h"

#include "../io/Camera.h"
#include "../modes/traits/TerrainTraits.h"
#include "../renderers/UiRenderer.h"
#include "TileRenderer.h"

UiLayerWireframe::UiLayerWireframe(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      sp_layer_(data::VboIdMain::kWireframeWindow),
      sp_frame_(data::VboIdMain::kWireframeWindowFrame),
      sp_points_({
          {data::VboIdMain::kWireframeWindowXneg},
          {data::VboIdMain::kWireframeWindowXpos},
          {data::VboIdMain::kWireframeWindowYneg},
          {data::VboIdMain::kWireframeWindowYpos},
          {data::VboIdMain::kWireframeWindowZneg},
          {data::VboIdMain::kWireframeWindowZpos},
      }),
      pos_points_({
          {-8.0f, 0.0f, 0.0f, 1.0f},
          {8.0f, 0.0f, 0.0f, 1.0f},
          {0.0f, -8.0f, 0.0f, 1.0f},
          {0.0f, 8.0f, 0.0f, 1.0f},
          {0.0f, 0.0f, -8.0f, 1.0f},
          {0.0f, 0.0f, 8.0f, 1.0f},
      }),
      hierarchy_(&sp_frame_, {&sp_layer_}) {
  Init();
}

void UiLayerWireframe::RenderLayerWireframe(TerrainTraits* terrain) {
  UpdateLayerWireframe(terrain);
  ui_shared_resources_.shader_sp_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  sp_frame_.Render();
  layer_tex_.Bind();
  sp_layer_.Render();
  // possible to bind these two, but need draw +-XYZ billboarding at first
}

void UiLayerWireframe::RenderPickingLayerWireframe() {
  sp_frame_.RenderPicking();
  sp_layer_.RenderPicking();
}

void UiLayerWireframe::UpdateLayerWireframe(TerrainTraits* terrain) {
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glViewport(0, 0, gWindowWidth / 4, gWindowHeight / 4);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ui_shared_resources_.glfw_context_.tile_renderer->terrain.RenderWireframe(
      terrain);
  ui_shared_resources_.glfw_context_.ui_renderer->RenderAxis(5.0f);

  ui_shared_resources_.shader_sp_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.tex_ui_.Bind();

  RenderAxis(0);  // x
  RenderAxis(2);  // y
  RenderAxis(4);  // z

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight);  /// restore
}

void UiLayerWireframe::RenderAxis(int first_idx) {
  // at least one of X+ and X- should be drawn (for each axis)
  bool neg_axis_skipped_ = false;
  int i = first_idx;
  auto mvp = GetPointMvpMatrix();
  glm::vec3 translate = GetBillboardTranslate(mvp, i);
  if (translate.z >= 1.0f) {
    neg_axis_skipped_ = true;
  } else {
    sp_points_[i].SetTranslate(
        glm::clamp(glm::vec2(translate.x, translate.y), -0.9f, 0.9f));
    sp_points_[i].Render();
  }
  ++i;
  mvp = GetPointMvpMatrix();
  translate = GetBillboardTranslate(mvp, i);
  if (translate.z < 1.0f) {
    sp_points_[i].SetTranslate(
        glm::clamp(glm::vec2(translate.x, translate.y), -0.9f, 0.9f));
    sp_points_[i].Render();
  } else if (neg_axis_skipped_) {
    sp_points_[i].SetTranslate(
        glm::normalize(glm::vec2(translate.x, translate.y)) * 0.9f);
    sp_points_[i].Render();
  }
}

glm::mat4 UiLayerWireframe::GetPointMvpMatrix() {
  auto model = glm::mat4(1.0f);
  auto map_scale =
      ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  model = glm::scale(model, glm::vec3(glm::sqrt(map_scale)));
  auto view =
      ui_shared_resources_.glfw_context_.camera->GetViewMatrix(map_scale);
  auto projection = ui_shared_resources_.glfw_context_.camera->GetProjMatrix();
  return projection * view * model;
}

glm::vec3 UiLayerWireframe::GetBillboardTranslate(glm::mat4 mvp, int idx) {
  glm::vec4 clipPos = mvp * pos_points_[idx];
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
  return ndc;
}

void UiLayerWireframe::UpdateRatio() {
  glm::vec2 frame_size =
      glm::vec2(sp_frame_.GetTopBorder() - sp_frame_.GetBottomBorder(),
                sp_frame_.GetRightBorder() - sp_frame_.GetLeftBorder());
  float start_ratio = frame_size.x / frame_size.y;
  float end_ratio =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  sp_frame_.SetExtraScale(end_ratio / start_ratio);
  sp_layer_.SetExtraScale(end_ratio /*end_ration*/);
  std::cout << "layer_ " << end_ratio << std::endl;
}

void UiLayerWireframe::InitFbo() {
  // TODO: fboDepth unhandled
  GLuint fbo_tex, fboDepth;
  int fboWidth = gWindowWidth / 4,
      fboHeight = gWindowHeight / 4;  // Preview resolution

  // Create color texture
  glGenTextures(1, &fbo_tex);
  glBindTexture(GL_TEXTURE_2D, fbo_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create depth buffer
  glGenRenderbuffers(1, &fboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth,
                        fboHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // Create framebuffer
  glGenFramebuffers(1, &layer_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         fbo_tex, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, fboDepth);

  // Check completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "FBO not complete!" << std::endl;
  }
  layer_tex_ = Texture(fbo_tex, 256, 256, GL_RGBA8);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UiLayerWireframe::Init() {
  InitFbo();
  UpdateRatio();
}

void UiLayerWireframe::DeInit() { glDeleteFramebuffers(1, &layer_fbo_); }
