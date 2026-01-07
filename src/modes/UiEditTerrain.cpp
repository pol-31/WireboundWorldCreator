#include "UiEditTerrain.h"

#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"
#include "../common/OpenGlUtility.h"

UiEditTerrain::UiEditTerrain(Tile& cur_tile,
    UiSharedResources& ui_shared_resources,
    TextRenderer& text_renderer,
    UiEditSlots& ui_edit_slots,
    UiEditConfigSlCfg& value_config,
    UiConfigWindow& ui_noise_config)
: IUiEdit(ui_edit_slots),
value_config_(value_config),
text_noise_invert_({text_renderer,
                          {data::VboIdMain::kTerrainEditLabelInvert},
                          data::TextId::kInvert}),
      text_noise_tiling_({text_renderer,
                          {data::VboIdMain::kTerrainEditLabelTiling},
                          data::TextId::kTiling}),
      text_noise_strength_({text_renderer,
                            {data::VboIdMain::kTerrainEditLabelStrength},
                            data::TextId::kStrength}),

      shader_merge_noises_("../shaders/noise_shaders/MergeNoises.comp"),
      shader_flatten_prep_("../shaders/generate_shaders/FlattenPrep.comp"),
      shader_flatten_step_("../shaders/generate_shaders/FlattenStep.comp"),
      shader_flatten_merge_("../shaders/generate_shaders/FlattenMerge.comp"),

      noise_perlin_("../shaders/noise_shaders/Perlin.comp",
                    {{10.0f, 10.0f, 20.0f}},
                    {{data::TextId::kScaleX, data::TextId::kScaleY,
                      data::TextId::kSeed}}),
      noise_cellular_("../shaders/noise_shaders/Cellular.comp",
                      {{10.0f, 10.0f, 1.0f, 20.0f}},
                      {{data::TextId::kScaleX, data::TextId::kScaleY,
                        data::TextId::kJitter, data::TextId::kSeed}}),
      noise_metaballs_("../shaders/noise_shaders/Metaballs.comp",
                       {{10.0f, 10.0f, 1.0f, 20.0f}},
                       {{data::TextId::kScaleX, data::TextId::kScaleY,
                         data::TextId::kJitter, data::TextId::kSeed}}),
      noise_fbm_grid_(
          "../shaders/noise_shaders/FbmGrid.comp",
          {{10.0f, 10.0f, 3.0f, 20.0f, 2.0f, 32.0f, 10.0f, 1.0f, 20.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kShift, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kWarpStrength,
            data::TextId::kOctaveFactor, data::TextId::kSeed}}),
      noise_fbm_multi_("../shaders/noise_shaders/FbmMulti.comp",
                       {{10.0f, 10.0f, 32.0f, 3.0f, 20.0f}},
                       {{data::TextId::kScaleX, data::TextId::kScaleY,
                         data::TextId::kLacunarity, data::TextId::kOctaves,
                         data::TextId::kSeed}}),
      noise_fbmd_perlin_("../shaders/noise_shaders/FbmdPerlin.comp",
                         {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f}},
                         {{data::TextId::kScaleX, data::TextId::kScaleY,
                           data::TextId::kOctaves, data::TextId::kGain,
                           data::TextId::kLacunarity, data::TextId::kSlopeness,
                           data::TextId::kOctaveFactor, data::TextId::kSeed}}),
      noise_fbm_warp_(
          "../shaders/noise_shaders/FbmWarp.comp",
          {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f, 5.0f, 5.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kSlopeness,
            data::TextId::kOctaveFactor, data::TextId::kSeed, data::TextId::kQ,
            data::TextId::kR}}),
      noise_fmb_perlin_warp_(
          "../shaders/noise_shaders/FbmPerlinWarp.comp",
          {{10.0f, 10.0f, 3.0f, 2.0f, 32.0f, 1.0f, 1.0f, 20.0f, 5.0f, 5.0f}},
          {{data::TextId::kScaleX, data::TextId::kScaleY,
            data::TextId::kOctaves, data::TextId::kGain,
            data::TextId::kLacunarity, data::TextId::kSlopeness,
            data::TextId::kOctaveFactor, data::TextId::kSeed, data::TextId::kQ,
            data::TextId::kR}}),
      noises_({&noise_perlin_, &noise_cellular_, &noise_metaballs_,
               &noise_fbm_grid_, &noise_fbm_multi_, &noise_fbmd_perlin_,
               &noise_fbm_warp_, &noise_fmb_perlin_warp_}),

      ui_shared_resources_(ui_shared_resources),
      ui_noise_config_(ui_noise_config) {
  Init();
  // value_config_.AttachToHierarchy(hierarchy_);
}

UiEditTerrain::UiEditTerrain(UiEditTerrain&& other) noexcept
    : IUiEdit(std::move(*this)),
text_noise_invert_(std::move(other.text_noise_invert_)),
      text_noise_tiling_(std::move(other.text_noise_tiling_)),
      text_noise_strength_(std::move(other.text_noise_strength_)),

      shader_merge_noises_(std::move(other.shader_merge_noises_)),
      shader_flatten_prep_(std::move(other.shader_flatten_prep_)),
      shader_flatten_step_(std::move(other.shader_flatten_step_)),
      shader_flatten_merge_(std::move(other.shader_flatten_merge_)),

value_config_(other.value_config_),

      noise_perlin_(std::move(other.noise_perlin_)),
      noise_cellular_(std::move(other.noise_cellular_)),
      noise_metaballs_(std::move(other.noise_metaballs_)),
      noise_fbm_grid_(std::move(other.noise_fbm_grid_)),
      noise_fbm_multi_(std::move(other.noise_fbm_multi_)),
      noise_fbmd_perlin_(std::move(other.noise_fbmd_perlin_)),
      noise_fbm_warp_(std::move(other.noise_fbm_warp_)),
      noise_fmb_perlin_warp_(std::move(other.noise_fmb_perlin_warp_)),
      noises_({&noise_perlin_, &noise_cellular_, &noise_metaballs_,
               &noise_fbm_grid_, &noise_fbm_multi_, &noise_fbmd_perlin_,
               &noise_fbm_warp_, &noise_fmb_perlin_warp_}),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(std::move(other.instances_)),
      ui_noise_config_(other.ui_noise_config_) {
  // value_config_.AttachToHierarchy(hierarchy_);
}

void UiEditTerrain::HideAll() {
  ui_.ForceHide();
  ui_noise_config_.ForceHide();
}

void UiEditTerrain::CreateInstance() {
  instances_.push_back(TerrainInstanceData{});
  instances_.back().data.hmap = Texture32F(details::gTerrainSize, GL_R32F);
  glClearTexImage(instances_.back().data.hmap.GetId(), 0, GL_RED, GL_FLOAT, nullptr);
  UpdateConfig();
}

void UiEditTerrain::UpdateConfig() {
  using namespace utility;
  float color_black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  Texture32F hmap(details::gTerrainSize, GL_R32F);
  glBindTexture(GL_TEXTURE_2D, hmap.GetId());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color_black);

  int size = details::gTerrainSize;
  hmap.Bind();
  glClearTexImage(hmap.GetId(), 0, GL_RED, GL_FLOAT, color_black);
  Texture32F tex_mesh(details::gTerrainSize, GL_R32F);
  for (int i = 0; i < instances_.size(); ++i) {
    if (!(*ui_.base_instances_)[i].do_show) {
      continue;
    }
    glClearTexImage(tex_mesh.GetId(), 0, GL_RED, GL_FLOAT, color_black);
    shader_flatten_prep_.Bind();
    glm::mat4 model = glm::mat4{1.0f};
    // scale to -512;512 I guess... (without it translation's wrong)
    model = glm::translate(model, instances_[i].translate * 16.0f);
    model *= glm::mat4_cast(instances_[i].rotate);
    model = glm::scale(model, instances_[i].scale);

    glm::mat4 inverseTransform = glm::inverse(model);
    glUniformMatrix4fv(4, 1, false, glm::value_ptr(inverseTransform));

    BindImageTexture(0, instances_[i].data.hmap, GL_READ_ONLY);
    BindImageTexture(1, tex_mesh, GL_WRITE_ONLY);
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    UnBindImageTexture(0, instances_[i].data.hmap, GL_READ_ONLY);
    UnBindImageTexture(1, tex_mesh, GL_WRITE_ONLY);

    /// --- flattening section ---
    /*int gradient_id = CalculateGradientId(instances_[i].rotate);
    if (false && gradient_id != -1) {
      shader_flatten_step_.Bind();
      GLuint changed_prev = -1;
      GLuint changed = 0;
      BindImageTexture(0, tex_mesh, GL_READ_WRITE);
      glUniform1i(3, gradient_id);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_atomic_modified_);
      while (changed_prev != changed) {
        glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        changed_prev = changed;
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint),
    &changed); std::cout << changed << " __" << std::endl;
      }
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      UnBindImageTexture(0, tex_mesh, GL_READ_WRITE);
    }*/

    shader_flatten_merge_.Bind();
    BindImageTexture(0, hmap, GL_READ_WRITE);
    BindImageTexture(1, tex_mesh, GL_READ_ONLY);
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    UnBindImageTexture(0, hmap, GL_READ_WRITE);
    UnBindImageTexture(1, tex_mesh, GL_READ_ONLY);
  }

  ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_
    .map_terrain_height_raw_ = std::move(hmap);
  ui_shared_resources_.glfw_context_.tile_renderer->UpdatePipeline();
  if (IsHmapNan()) {
    std::cerr << "unnable to generate noises, change the config" << std::endl;
  }
}

void UiEditTerrain::SetInstanceId(int id) {
  ui_.SetInstance(id);
  const auto& noise_data = instances_[id].data;
  noise_perlin_.SetConfig(
      &noise_data.perlin,
      reinterpret_cast<const float*>(
          static_cast<const NoisePerlinData*>(&noise_data.perlin)));
  noise_cellular_.SetConfig(
      &noise_data.cellular,
      reinterpret_cast<const float*>(
          static_cast<const NoiseCellularData*>(&noise_data.cellular)));
  noise_metaballs_.SetConfig(
      &noise_data.metaballs,
      reinterpret_cast<const float*>(
          static_cast<const NoiseMetaballsData*>(&noise_data.metaballs)));
  noise_fbm_grid_.SetConfig(
      &noise_data.fbm_grid,
      reinterpret_cast<const float*>(
          static_cast<const NoiseFbmGridData*>(&noise_data.fbm_grid)));
  noise_fbm_multi_.SetConfig(
      &noise_data.fbm_multi,
      reinterpret_cast<const float*>(
          static_cast<const NoiseFbmMultiData*>(&noise_data.fbm_multi)));
  noise_fbmd_perlin_.SetConfig(
      &noise_data.fbmd_perlin,
      reinterpret_cast<const float*>(
          static_cast<const NoiseFbmdPerlinData*>(&noise_data.fbmd_perlin)));
  noise_fbm_warp_.SetConfig(
      &noise_data.fbm_warp,
      reinterpret_cast<const float*>(
          static_cast<const NoiseFbmWarpData*>(&noise_data.fbm_warp)));
  noise_fmb_perlin_warp_.SetConfig(
      &noise_data.fbm_perlin_warp,
      reinterpret_cast<const float*>(static_cast<const NoiseFbmPerlinWarpData*>(
          &noise_data.fbm_perlin_warp)));
  UpdateConfig();
}

void UiEditTerrain::RemoveInstance(GLuint id) {
  instances_.erase(instances_.begin() + id);
  UpdateConfig();
}

void UiEditTerrain::Reset() {
  instances_.clear();
  UpdateConfig();
}

void UiEditTerrain::Generate() {
  glm::vec2 resolution = glm::vec2{details::gTerrainSize};
  NoiseTerrainData terrain_data;
  terrain_data.hmap = Texture32F(resolution.x, GL_R32F);
  float color_black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearTexImage(terrain_data.hmap.GetId(), 0, GL_RED, GL_FLOAT, color_black);

  Texture32F tex_noise;
  glClearTexImage(tex_noise.GetId(), 0, GL_RED, GL_FLOAT, color_black);
  if (noises_[0]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_perlin_.Generate(reinterpret_cast<float*>(
        static_cast<NoisePerlinData*>(&terrain_data.perlin)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[0]->GetBaseData());
  }
  if (noises_[1]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_cellular_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseCellularData*>(&terrain_data.cellular)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[1]->GetBaseData());
  }
  if (noises_[2]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_metaballs_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseMetaballsData*>(&terrain_data.metaballs)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[2]->GetBaseData());
  }
  if (noises_[3]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_fbm_grid_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseFbmGridData*>(&terrain_data.fbm_grid)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[3]->GetBaseData());
  }
  if (noises_[4]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_fbm_multi_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseFbmMultiData*>(&terrain_data.fbm_multi)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[4]->GetBaseData());
  }
  if (noises_[5]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_fbmd_perlin_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseFbmdPerlinData*>(&terrain_data.fbmd_perlin)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[5]->GetBaseData());
  }
  if (noises_[6]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_fbm_warp_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseFbmWarpData*>(&terrain_data.fbm_warp)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[6]->GetBaseData());
  }
  if (noises_[7]->GetBaseData()->strength != 0.0f) {
    tex_noise = noise_fmb_perlin_warp_.Generate(reinterpret_cast<float*>(
        static_cast<NoiseFbmPerlinWarpData*>(&terrain_data.fbm_perlin_warp)));
    MergeLayers(terrain_data.hmap, tex_noise, noises_[7]->GetBaseData());
  }
  std::cout << "Generation :: DONE" << std::endl;
  instances_[*ui_.selected_id_].data = std::move(terrain_data);
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
  int pressed_line_id =
      GetUiEditEntryId(height, noises_.size(),
                       ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_,
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

void UiEditTerrain::Release() {
  value_config_.Release();
}

void UiEditTerrain::Render(float height) {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = height / (noises_.size() + 5);  // +pads
  for (int i = 0; i < noises_.size(); ++i) {
    auto data = noises_[i]->GetBaseData();
    value_config_.Render(
        data->strength, data->do_invert, data->do_tiling, next_offset,
        value_config_.pressed_strength_id_ == i, noises_[i]->GetTextId());
    next_offset.y -= entry_height;
  }
  text_noise_invert_.Render();
  text_noise_tiling_.Render();
  text_noise_strength_.Render();
}

void UiEditTerrain::RenderPicking(float height) {
  value_config_.RenderPicking(height, noises_.size());
  ui_shared_resources_.shader_sp_picking_.Bind();
  text_noise_invert_.RenderPicking();
  text_noise_tiling_.RenderPicking();
  text_noise_strength_.RenderPicking();
}

TerrainInstanceData& UiEditTerrain::GetInstanceData() noexcept {
  return instances_[*ui_.selected_id_];
}

void UiEditTerrain::RenderGraph() {
  int id = *ui_.selected_id_;
  if (id == -1) {
    return;
  }
  /// selected layer wireframe
  ui_shared_resources_.glfw_context_.tile_renderer->terrain.RenderWireframe(
      &instances_[id], &(*ui_.base_instances_)[id]);
  auto color_invert = glm::vec4(1.0f) - (*ui_.base_instances_)[id].color;
  color_invert.w = 1.0f;

  glm::vec4 pivot_pos = GetLayerCentre() - pivot_offset_;
  auto map_scale =
      ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  pivot_pos = glm::vec4{pivot_pos.x * map_scale, pivot_pos.y * map_scale,
                        pivot_pos.z * map_scale, 1.0f};
  ui_shared_resources_.glfw_context_.ui_renderer->RenderWorldOrigin(
      pivot_pos, color_invert);

  /// wireframe left bottom
  auto& ui_layer_wireframe =
      ui_shared_resources_.glfw_context_.ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderLayerWireframe(
    &instances_[id], &(*ui_.base_instances_)[id]);
}

void UiEditTerrain::SetPivotPosition(GLuint pressed_id) {
  if (pressed_id >= details::kIdOffsetWater || *ui_.selected_id_ == -1) {
    return;
  }
  glm::vec4 position(1.0f);
  glm::uvec2 pos(pressed_id >> 10, pressed_id & 1023);
  position.x = pos.x / 16.0f - 32.0f;
  position.z = pos.y / 16.0f - 32.0f;
  position.y = ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_
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

void UiEditTerrain::DeInit() {
  glDeleteBuffers(1, &ssbo_atomic_modified_);
}

glm::vec4 UiEditTerrain::GetLayerCentre() {
  int id = *ui_.selected_id_;
  return {instances_[id].translate.x,
          instances_[id].translate.y,
          instances_[id].translate.z, 1.0f};
}

bool UiEditTerrain::IsHmapNan() const noexcept {
  /// we should've make "smart" random, but now some params in some
  /// noises contradicts to some other params like "octaves" and "scale",
  /// so we do the stupid approach - rerun.
  /// (I calculated it on my own - in average it's 6/30 with "bad" result
  return std::isnan(ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_
                        .terrain_heights_[0]);
}

void UiEditTerrain::MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                                const NoiseDataBase* config) {
  std::cout << "merged with " << config->strength << ' ' << std::boolalpha
            << config->do_invert << ' ' << config->do_tiling << std::endl;
  shader_merge_noises_.Bind();
  //  glUniform1i(0, static_cast<int>(config->do_invert));
  // TODO: as well as transform_matrix:
  //  glUniform1i(1, static_cast<int>(noise_data->do_tiling));
  //  glUniform1f(2, config->strength / 8.0f);
  glUniform1f(2, config->strength);
  //  glUniform1f(2, .0f);

  glBindImageTexture(0, bottom_layer.GetId(), 0, GL_FALSE, 0, GL_READ_WRITE,
                     bottom_layer.GetFormat());
  glBindImageTexture(1, top_layer.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     top_layer.GetFormat());

  int size = details::gTerrainSize;
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
