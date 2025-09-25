#include "UiLayerWireframe.h"

#include "../modes/TerrainInstanceData.h"
#include "TileRenderer.h"

#include "../io/Cameras.h"

UiLayerWireframe::UiLayerWireframe(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      layer_(data::VboIdMain::kWireframeWindow),
      frame_(data::VboIdMain::kWireframeWindowFrame),
      shader_billboard_("../shaders/Billboard.vert",
                        "../shaders/Billboard.frag"),
      hierarchy_(&frame_) {
  hierarchy_ = UiHierarchy(&frame_, &layer_);
  Init();
}

UiLayerWireframe::UiLayerWireframe(UiLayerWireframe&& other) noexcept
    : ui_shared_resources_(other.ui_shared_resources_),
      layer_(std::move(other.layer_)),
      frame_(std::move(other.frame_)),
      layer_tex_(std::move(other.layer_tex_)),
      layer_fbo_(other.layer_fbo_),
      billboard_vao_(other.billboard_vao_),
      billboard_vbo_(other.billboard_vbo_),
      shader_billboard_(std::move(other.shader_billboard_)),
      hierarchy_(std::move(other.hierarchy_)) {
  other.layer_fbo_ = 0;
  other.billboard_vao_ = 0;
  other.billboard_vbo_ = 0;
  hierarchy_ = UiHierarchy(&frame_, &layer_);
}

void UiLayerWireframe::RenderLayerWireframe(
    TerrainInstanceData* terrain) {
  UpdateLayerWireframe(terrain);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  frame_.Render();
  layer_tex_.Bind();
  layer_.Render();
  // possible to bind these two, but need draw +-XYZ billboarding at first
}

void UiLayerWireframe::RenderPickingLayerWireframe() {
  frame_.RenderPicking();
  layer_.RenderPicking();
}

void UiLayerWireframe::UpdateLayerWireframe(
    TerrainInstanceData* terrain) {
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glViewport(0, 0, gWindowWidth / 4, gWindowHeight / 4);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->terrain.RenderWireframe(terrain);

  shader_billboard_.Bind();
  glBindVertexArray(billboard_vao_);
  auto view = ui_shared_resources_.global_glfw_callback_data_
                  .camera->GetViewMatrix();
  auto projection = ui_shared_resources_.global_glfw_callback_data_
                        .camera->GetProjMatrix();
  std::array<glm::vec3, 6> spritePositions = {{
      {-64.0f, 0.0f, 0.0f},
      {0.0f, -64.0f, 0.0f},
      {0.0f, 0.0f, -64.0f},
      {64.0f, 0.0f, 0.0f},
      {0.0f, 64.0f, 0.0f},
      {0.0f, 0.0f, 64.0f},
  }};
//  glDisable(GL_DEPTH_TEST);
  for (int i = 0; i < 6; i++) {
    glm::mat4 mvp = GetBillboardMatrix(spritePositions[i], view, projection);
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(mvp));
    glUniform1i(1, 0);
    glActiveTexture(GL_TEXTURE0);
    ui_shared_resources_.tex_ui_.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
  }
//  glEnable(GL_DEPTH_TEST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight); /// restore
}

glm::mat4 UiLayerWireframe::GetBillboardMatrix(
    const glm::vec3& position, const glm::mat4& view,
    const glm::mat4& projection) {
  auto transform = glm::mat4(1.0f);
  auto map_scale = ui_shared_resources_.global_glfw_callback_data_.
                   tile_renderer->cur_tile_.map_scale;
  transform = glm::scale(transform, glm::vec3(map_scale));
  transform = glm::translate(transform, position * map_scale);
//  transform = glm::scale(transform, glm::vec3(.2f / map_scale));
  glm::mat4 model = transform;

  // Extract camera rotation
  glm::mat4 billboard = view;
  billboard[3] = glm::vec4(0,0,0,1); // remove translation
  billboard = glm::transpose(billboard); // invert rotation part

  return projection * view * model * billboard;
}

void UiLayerWireframe::UpdateRatio() {
  glm::vec2 frame_size =
      glm::vec2(frame_.GetTopBorder() - frame_.GetBottomBorder(),
                frame_.GetRightBorder() - frame_.GetLeftBorder());
  float start_ratio = frame_size.x / frame_size.y;
  float end_ratio =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  frame_.SetExtraScale(end_ratio / start_ratio);
  layer_.SetExtraScale(end_ratio/*end_ration*/);
  std::cout << "layer_ " << end_ratio << std::endl;
}

void UiLayerWireframe::InitFbo() {
  //TODO: fboDepth unhandled
  GLuint fbo_tex, fboDepth;
  int fboWidth = gWindowWidth / 4, fboHeight = gWindowHeight / 4; // Preview resolution

  // Create color texture
  glGenTextures(1, &fbo_tex);
  glBindTexture(GL_TEXTURE_2D, fbo_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create depth buffer
  glGenRenderbuffers(1, &fboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // Create framebuffer
  glGenFramebuffers(1, &layer_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, fbo_tex, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, fboDepth);

  // Check completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "FBO not complete!" << std::endl;
  }
  layer_tex_ = Texture(fbo_tex, 256, 256, GL_RGBA8);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UiLayerWireframe::InitBillboards() {
  int id = (static_cast<int>(data::VboIdMain::kWireframeWindowXpos)
            - details::kIdOffsetUi) * 16;

  float quadVertices[] = {
      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],

      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],

      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],

      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],

      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],

      0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 2], data::kUiVboDataMain[id + 3],
      0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 6], data::kUiVboDataMain[id + 7],
      -0.5f, -0.5f, 0.0f,  data::kUiVboDataMain[id + 10], data::kUiVboDataMain[id + 11],
      -0.5f,  0.5f, 0.0f,  data::kUiVboDataMain[id + 14], data::kUiVboDataMain[id + 15],
  };
  for (int i = 0; i < 6 * 4; ++i) {
    quadVertices[i * 5 + 3] = data::kUiVboDataMain[id + 2 + 4 * i];
    quadVertices[i * 5 + 4] = data::kUiVboDataMain[id + 3 + 4 * i];
  }

  glGenVertexArrays(1, &billboard_vao_);
  glBindVertexArray(billboard_vao_);

  glGenBuffers(1, &billboard_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void UiLayerWireframe::Init() {
  InitFbo();
  UpdateRatio();
  InitBillboards();
}

void UiLayerWireframe::DeInit() {
  glDeleteFramebuffers(1, &layer_fbo_);

  glDeleteVertexArrays(1, &billboard_vao_);
  glDeleteBuffers(1, &billboard_vbo_);
}
