#include "UiEditTerrain.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common/OpenGlUtility.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer__Deprecated.h"

UiEditTerrain::UiEditTerrain(Tile& cur_tile, UiRenderData& render_data,
                             TextRenderer& text_renderer,
                             UiEditSlots& ui_edit_slots,
                             UiEditConfigSlCfg& value_config,
                             UiConfigWindow& ui_noise_config)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      text_noise_invert_({text_renderer,
                          {data::UiId::kTerrainEditLabelInvert},
                          data::TextId::kInvert}),
      text_noise_tiling_({text_renderer,
                          {data::UiId::kTerrainEditLabelTiling},
                          data::TextId::kTiling}),
      text_noise_strength_({text_renderer,
                            {data::UiId::kTerrainEditLabelStrength},
                            data::TextId::kStrength}),

      shader_merge_noises_("../shaders/noise_shaders/MergeNoises.comp", {}),
      shader_flatten_prep_("../shaders/generate_shaders/FlattenPrep.comp", {}),
      shader_flatten_step_("../shaders/generate_shaders/FlattenStep.comp", {}),
      shader_flatten_merge_("../shaders/generate_shaders/FlattenMerge.comp",
                            {}),

      gen_perlin_("../shaders/noise_shaders/Perlin.comp",
                  {{10.0f, 10.0f, 20.0f}},
                  {{data::TextId::kScaleX, data::TextId::kScaleY,
                    data::TextId::kSeed}}),
      gen_cellular_("../shaders/noise_shaders/Cellular.comp",
                    {{10.0f, 10.0f, 1.0f, 20.0f}},
                    {{data::TextId::kScaleX, data::TextId::kScaleY,
                      data::TextId::kJitter, data::TextId::kSeed}}),
      gen_metaballs_("../shaders/noise_shaders/Metaballs.comp",
                     {{10.0f, 10.0f, 1.0f, 20.0f}},
                     {{data::TextId::kScaleX, data::TextId::kScaleY,
                       data::TextId::kJitter, data::TextId::kSeed}}),
      gen_fbm_grid_(
          "../shaders/noise_shaders/FbmGrid.comp",
          {{10.0f, 10.0f, 3.0f, 20.0f, 2.0f, 32.0f, 10.0f, 1.0f, 20.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kShift, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kWarpStrength,
            data::TextId::kOctaveFactor, data::TextId::kSeed}}),
      gen_fbm_multi_("../shaders/noise_shaders/FbmMulti.comp",
                     {{10.0f, 10.0f, 32.0f, 3.0f, 20.0f}},
                     {{data::TextId::kScaleX, data::TextId::kScaleY,
                       data::TextId::kLacunarity, data::TextId::kOctaves,
                       data::TextId::kSeed}}),
      gen_fbmd_perlin_("../shaders/noise_shaders/FbmdPerlin.comp",
                       {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f}},
                       {{data::TextId::kScaleX, data::TextId::kScaleY,
                         data::TextId::kOctaves, data::TextId::kGain,
                         data::TextId::kLacunarity, data::TextId::kSlopeness,
                         data::TextId::kOctaveFactor, data::TextId::kSeed}}),
      gen_fbm_warp_(
          "../shaders/noise_shaders/FbmWarp.comp",
          {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f, 5.0f, 5.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kSlopeness,
            data::TextId::kOctaveFactor, data::TextId::kSeed, data::TextId::kQ,
            data::TextId::kR}}),
      gen_fbm_perlin_warp_(
          "../shaders/noise_shaders/FbmPerlinWarp.comp",
          {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f, 5.0f, 5.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kSlopeness,
            data::TextId::kOctaveFactor, data::TextId::kSeed, data::TextId::kQ,
            data::TextId::kR}}),
      noises_({&gen_perlin_, &gen_cellular_, &gen_metaballs_, &gen_fbm_grid_,
               &gen_fbm_multi_, &gen_fbmd_perlin_, &gen_fbm_warp_,
               &gen_fbm_perlin_warp_}),

      render_data_(render_data),
      ui_noise_config_(ui_noise_config) {
  Init();
}

void UiEditTerrain::HideAll() {
  ui_.ForceHide();
  ui_noise_config_.ForceHide();
}

void UiEditTerrain::CreateInstance() {
  Data().push_back(TerrainTraits{});
  Data().back().data.hmap = Texture32F(Texture::Type::TerrainR32F);
  Data().back().extra_heights = Texture32F(Texture::Type::TerrainR32F);
  UpdateConfig();
}

void UiEditTerrain::UpdateConfig() {
  using namespace utility;
  int size = details::gTerrainSize;
  Texture32F hmap(Texture::Type::TerrainR32F);
  glBindTexture(GL_TEXTURE_2D, hmap.GetId());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float color_black[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color_black);

  Texture32F tex_mesh(Texture::Type::TerrainR32F);
  for (int i = 0; i < Data().size(); ++i) {
    if (!Data()[i].do_show) {
      continue;
    }
    tex_mesh.Clear();
    shader_flatten_prep_.Bind();
    glm::mat4 model = glm::mat4{1.0f};
    // scale to -512;512 I guess... (without it translation's wrong)
    model = glm::translate(model, Data()[i].translate * 16.0f);
    model *= glm::mat4_cast(Data()[i].rotate);
    model = glm::scale(model, Data()[i].scale);

    glm::mat4 inverseTransform = glm::inverse(model);
    glUniformMatrix4fv(4, 1, false, glm::value_ptr(inverseTransform));

    Data()[i].data.hmap.BindImage(0, GL_READ_ONLY);
    tex_mesh.BindImage(1, GL_WRITE_ONLY);
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    /// --- flattening section ---
    // int gradient_id = CalculateGradientId(Data()[i].rotate);
    // if (false && gradient_id != -1) {
    //   shader_flatten_step_.Bind();
    //   GLuint changed_prev = -1;
    //   GLuint changed = 0;
    //   BindImageTexture(0, tex_mesh, GL_READ_WRITE);
    //   glUniform1i(3, gradient_id);
    //   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_atomic_modified_);
    //   while (changed_prev != changed) {
    //     glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    //     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //     changed_prev = changed;
    //     glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint),
    // &changed); std::cout << changed << " __" << std::endl;
    //   }
    //   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //   UnBindImageTexture(0, tex_mesh, GL_READ_WRITE);
    // }

    shader_flatten_merge_.Bind();
    hmap.BindImage(0, GL_READ_WRITE);
    tex_mesh.BindImage(1, GL_READ_ONLY);
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    std::vector<float> heights_in(size * size, 0.0f);
    glGetTextureImage(hmap.GetId(), 0, GL_RED,  // valid data
                      GL_FLOAT, size * size * sizeof(float), heights_in.data());

    auto& extra_heights = Data()[i].extra_heights;

    shader_merge_noises_.Bind();
    glUniform1f(2, 1.0f);
    hmap.BindImage(0, GL_READ_WRITE);
    extra_heights.BindImage(1, GL_READ_ONLY);
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }

  render_data_.glfw_context_.tile_renderer->cur_tile_.map_terrain_height_raw_ =
      std::move(hmap);
  render_data_.glfw_context_.tile_renderer->UpdatePipeline();
  if (IsHmapNan()) {
    std::cerr << "unnable to generate noises, change the config" << std::endl;
  }
}

void UiEditTerrain::SetInstanceId(int id) {
  ui_.SetInstance(id);
  auto& noise_data = Data()[id].data;
  gen_perlin_.SetConfig(&noise_data.perlin, {&noise_data.perlin.scale_x, 3});
  gen_cellular_.SetConfig(&noise_data.cellular,
                          {&noise_data.cellular.scale_x, 4});
  gen_metaballs_.SetConfig(&noise_data.metaballs,
                           {&noise_data.metaballs.scale_x, 4});
  gen_fbm_grid_.SetConfig(&noise_data.fbm_grid,
                          {&noise_data.fbm_grid.scale_x, 9});
  gen_fbm_multi_.SetConfig(&noise_data.fbm_multi,
                           {&noise_data.fbm_multi.scale_x, 5});
  gen_fbmd_perlin_.SetConfig(&noise_data.fbmd_perlin,
                             {&noise_data.fbmd_perlin.scale_x, 8});
  gen_fbm_warp_.SetConfig(&noise_data.fbm_warp,
                          {&noise_data.fbm_warp.scale_x, 10});
  gen_fbm_perlin_warp_.SetConfig(&noise_data.fbm_perlin_warp,
                                 {&noise_data.fbm_perlin_warp.scale_x, 10});
}

void UiEditTerrain::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditTerrain::Reset() {
  render_data_.glfw_context_.tile_renderer->cur_tile_.ResetTerrain();
  Data().clear();
  UpdateConfig();
}

void UiEditTerrain::Generate() {
  auto& data = Data()[selected_id_].data;
  data.hmap = Texture32F(Texture::Type::TerrainR32F);
  Texture32F tex_noise = Texture32F(Texture::Type::TerrainR32F);
  MergeLayers(data.hmap, tex_noise, &data.perlin, &gen_perlin_);
  MergeLayers(data.hmap, tex_noise, &data.cellular, &gen_cellular_);
  MergeLayers(data.hmap, tex_noise, &data.metaballs, &gen_metaballs_);
  MergeLayers(data.hmap, tex_noise, &data.fbm_grid, &gen_fbm_grid_);
  MergeLayers(data.hmap, tex_noise, &data.fbm_multi, &gen_fbm_multi_);
  MergeLayers(data.hmap, tex_noise, &data.fbmd_perlin, &gen_fbmd_perlin_);
  MergeLayers(data.hmap, tex_noise, &data.fbm_warp, &gen_fbm_warp_);
  MergeLayers(data.hmap, tex_noise, &data.fbm_perlin_warp,
              &gen_fbm_perlin_warp_);
  std::cout << "Generation :: DONE" << std::endl;
  UpdateConfig();
}

void UiEditTerrain::GenerateAll() {
  auto& all_data = Data();
  for (int i = 0; i < all_data.size(); ++i) {
    auto& data = all_data[i].data;
    gen_perlin_.SetConfig(&data.perlin, {&data.perlin.scale_x, 3});
    gen_cellular_.SetConfig(&data.cellular, {&data.cellular.scale_x, 4});
    gen_metaballs_.SetConfig(&data.metaballs, {&data.metaballs.scale_x, 4});
    gen_fbm_grid_.SetConfig(&data.fbm_grid, {&data.fbm_grid.scale_x, 9});
    gen_fbm_multi_.SetConfig(&data.fbm_multi, {&data.fbm_multi.scale_x, 5});
    gen_fbmd_perlin_.SetConfig(&data.fbmd_perlin,
                               {&data.fbmd_perlin.scale_x, 8});
    gen_fbm_warp_.SetConfig(&data.fbm_warp, {&data.fbm_warp.scale_x, 10});
    gen_fbm_perlin_warp_.SetConfig(&data.fbm_perlin_warp,
                                   {&data.fbm_perlin_warp.scale_x, 10});
    data.hmap = Texture32F(Texture::Type::TerrainR32F);
    Texture32F tex_noise;
    MergeLayers(data.hmap, tex_noise, &data.perlin, &gen_perlin_);
    MergeLayers(data.hmap, tex_noise, &data.cellular, &gen_cellular_);
    MergeLayers(data.hmap, tex_noise, &data.metaballs, &gen_metaballs_);
    MergeLayers(data.hmap, tex_noise, &data.fbm_grid, &gen_fbm_grid_);
    MergeLayers(data.hmap, tex_noise, &data.fbm_multi, &gen_fbm_multi_);
    MergeLayers(data.hmap, tex_noise, &data.fbmd_perlin, &gen_fbmd_perlin_);
    MergeLayers(data.hmap, tex_noise, &data.fbm_warp, &gen_fbm_warp_);
    MergeLayers(data.hmap, tex_noise, &data.fbm_perlin_warp,
                &gen_fbm_perlin_warp_);
  }
  UpdateConfig();
}

void UiEditTerrain::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  for (auto n : noises_) {
    n->Randomize(ui_.random_generator_, dist_float, dist_bool);
  }
  noises_[5]->GetBaseData()->strength = 0.1f;
  noises_[6]->GetBaseData()->strength = 0.0f;
  noises_[7]->GetBaseData()->strength = 0.0f;
  Generate();
  if (IsHmapNan()) {
    RandomGenerate();  // simply rerun
  }
}

bool UiEditTerrain::Press(int id, float height) {
  value_config_.ResetTransform();
  int pressed_line_id = GetUiEditEntryId(
      height, noises_.size(), render_data_.glfw_context_.cursor_pos_tex_norm_,
      value_config_.btn_config_.GetTopBorder());
  if (id == value_config_.btn_config_.GetId()) {
    ui_noise_config_.SetNoise(noises_[pressed_line_id]->GetValueSpan(),
                              noises_[pressed_line_id]->GetTextIdSpan(),
                              noises_[pressed_line_id]->GetTextId());
    ui_noise_config_.Show();
  } else if (id == value_config_.sl_strength_.GetId()) {
    value_config_.pressed_strength_id_ = pressed_line_id;
    value_config_.sl_strength_.Press();
  } else if (id == value_config_.tg1_.GetId()) {
    noises_[pressed_line_id]->ToggleDoInvert();
  } else if (id == value_config_.tg2_.GetId()) {
    noises_[pressed_line_id]->ToggleDoTiling();
  } else {
    return false;
  }
  return true;
}

void UiEditTerrain::Release() { value_config_.Release(); }

void UiEditTerrain::Render(float height) {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = height / (noises_.size() + 5);  // +pads
  for (int i = 0; i < noises_.size(); ++i) {
    auto data = noises_[i]->GetBaseData();
    value_config_.Render(data->strength, data->do_invert, data->do_tiling,
                         next_offset, value_config_.pressed_strength_id_ == i,
                         noises_[i]->GetTextId());
    next_offset.y -= entry_height;
  }
  text_noise_invert_.Render();
  text_noise_tiling_.Render();
  text_noise_strength_.Render();
}

void UiEditTerrain::RenderPicking(float height) {
  value_config_.RenderPicking(height, noises_.size());
  render_data_.shader_sp_picking_.Bind();
  text_noise_invert_.RenderPicking();
  text_noise_tiling_.RenderPicking();
  text_noise_strength_.RenderPicking();
}

TerrainTraits& UiEditTerrain::GetInstanceData() noexcept {
  return Data()[selected_id_];
}

void UiEditTerrain::RenderGraph() {
  if (selected_id_ == -1) {
    return;
  }
  /// selected layer wireframe
  render_data_.glfw_context_.tile_renderer->terrain.RenderWireframe(
      &Data()[selected_id_]);
  auto color_invert = glm::vec4(1.0f) - Data()[selected_id_].color;
  color_invert.w = 1.0f;

  glm::vec4 pivot_pos = GetLayerCentre() - pivot_offset_;
  auto map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  pivot_pos = glm::vec4{pivot_pos.x * map_scale, pivot_pos.y * map_scale,
                        pivot_pos.z * map_scale, 1.0f};
  render_data_.glfw_context_.ui_renderer->RenderWorldOrigin(pivot_pos,
                                                            color_invert);

  /// wireframe left bottom
  render_data_.glfw_context_.ui_renderer->ui_layer_wireframe_
      .RenderLayerWireframe(&Data()[selected_id_]);
}

void UiEditTerrain::SetPivotPosition(GLuint pressed_id) {
  if (pressed_id >= details::kIdOffsetWater || selected_id_ == -1) {
    return;
  }
  glm::vec4 position(1.0f);
  glm::uvec2 pos(pressed_id >> 10, pressed_id & 1023);
  position.x = pos.x / 16.0f - 32.0f;
  position.z = pos.y / 16.0f - 32.0f;
  position.y = render_data_.glfw_context_.tile_renderer->cur_tile_
                   .terrain_heights_[pressed_id];
  auto layer_centre = GetLayerCentre();
  pivot_offset_ = layer_centre - position;
  pivot_offset_ =
      glm::vec4{pivot_offset_.x, pivot_offset_.y, pivot_offset_.z, 0.0f};
}

int UiEditTerrain::CalculateGradientId(const glm::vec3& rotation) {
  std::array<glm::vec3, 4> corners = {
      glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{0.5f, -0.5f, 0.0f},
      glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{-0.5f, 0.5f, 0.0f}};
  glm::mat4 R(1.0f);
  R = glm::rotate(R, rotation.x, glm::vec3(1, 0, 0));
  R = glm::rotate(R, rotation.y, glm::vec3(0, 1, 0));
  R = glm::rotate(R, rotation.z, glm::vec3(0, 0, 1));
  float h[4];
  for (int i = 0; i < 4; ++i) {
    h[i] = glm::vec3(R * glm::vec4(corners[i], 1.0f)).z;
  }
  float left = (h[0] + h[3]) * 0.5f;
  float right = (h[1] + h[2]) * 0.5f;
  float bottom = (h[0] + h[1]) * 0.5f;
  float top = (h[2] + h[3]) * 0.5f;
  glm::vec2 slope(right - left, top - bottom);
  int dx = (slope.x > 0.1f) - (slope.x < -0.1f);
  int dy = (slope.y > 0.1f) - (slope.y < -0.1f);
  int id = (dy + 1) * 3 + (dx + 1);
  // 0 1 2    0 7 6
  // 3 4 5 -> 1 -1 5 (gpu has slightly different ids
  // 6 7 8    2 3 4
  int shader_id[] = {0, 7, 6, 1, -1, 5, 2, 3, 4};
  return shader_id[id];
}

void UiEditTerrain::Init() {
  glGenBuffers(1, &ssbo_atomic_modified_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_atomic_modified_);
  GLuint zero = 0;
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), &zero,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_atomic_modified_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void UiEditTerrain::DeInit() { glDeleteBuffers(1, &ssbo_atomic_modified_); }

glm::vec4 UiEditTerrain::GetLayerCentre() {
  return {Data()[selected_id_].translate.x, Data()[selected_id_].translate.y,
          Data()[selected_id_].translate.z, 1.0f};
}

bool UiEditTerrain::IsHmapNan() const noexcept {
  /// we should've make "smart" random, but now some params in some
  /// noises contradicts to some other params like "octaves" and "scale",
  /// so we do the stupid approach - rerun.
  /// (I calculated it on my own - in average it's 6/30 with "bad" result
  return std::isnan(
      render_data_.glfw_context_.tile_renderer->cur_tile_.terrain_heights_[0]);
}

void UiEditTerrain::MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                                const NoiseDataBase* config,
                                ITerrainNoise* noise_gen) {
  if (config->strength == 0) {
    return;
  }
  top_layer = noise_gen->Generate();
  std::cout << "merged with " << config->strength << ' ' << std::boolalpha
            << config->do_invert << ' ' << config->do_tiling << std::endl;
  shader_merge_noises_.Bind();
  //  glUniform1i(0, static_cast<int>(config->do_invert));
  // TODO: as well as transform_matrix:
  //  glUniform1i(1, static_cast<int>(noise_data->do_tiling));
  glUniform1f(2, config->strength);

  using namespace utility;
  bottom_layer.BindImage(0, GL_READ_WRITE);
  top_layer.BindImage(1, GL_READ_ONLY);
  int size = details::gTerrainSize;
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

std::vector<TerrainTraits>& UiEditTerrain::Data() {
  return render_data_.glfw_context_.tile_renderer->cur_tile_.terrain_data;
}
