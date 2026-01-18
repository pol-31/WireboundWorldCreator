#ifndef WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H
#define WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "../common/TextRenderer.h"
#include "../core/Tile.h"
#include "../core/UiConfigWindow.h"
#include "TerrainInstanceData.h"
#include "TerrainNoiseData.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"
#include "UiTerrainConfig.h"

class UiEditTerrain final : public IUiEdit {
 public:
  UiEditTerrain(Tile& cur_tile, UiSharedResources& ui_shared_resources,
                TextRenderer& text_renderer, UiEditSlots& ui_edit_slots,
                UiEditConfigSlCfg& value_config,
                UiConfigWindow& ui_noise_config);

  ~UiEditTerrain() override { DeInit(); }

  UiEditTerrain(UiEditTerrain&& other) noexcept;

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  void RemoveInstance(GLuint id) override;

  void Reset() override;

  void Generate() override;

  void RandomGenerate() override;

  bool Press(int id, float height) override;

  void Release() override;

  void Render(float height) override;

  void RenderPicking(float height) override;

  [[nodiscard]] TerrainInstanceData& GetInstanceData() noexcept;

  void RenderGraph();

  void SetPivotPosition(GLuint pressed_id);

 private:
  static int CalculateGradientId(const glm::vec3& rotation);

  void Init();

  void DeInit();

  glm::vec4 GetLayerCentre();

  [[nodiscard]] bool IsHmapNan() const noexcept;

  void MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                   const NoiseDataBase* noise);

  std::vector<TerrainInstanceData> instances_;
  UiEditConfigSlCfg& value_config_;
  UiConfigWindow& ui_noise_config_;

  GLuint ssbo_atomic_modified_;
  glm::vec4 pivot_offset_ = glm::vec4(0.0f);
  Shader shader_merge_noises_;
  Shader shader_flatten_prep_;
  Shader shader_flatten_step_;
  Shader shader_flatten_merge_;

  UiTextModeId text_noise_invert_;
  UiTextModeId text_noise_tiling_;
  UiTextModeId text_noise_strength_;

  TerrainNoisePerlin noise_perlin_;
  TerrainNoiseCellular noise_cellular_;
  TerrainNoiseMetaballs noise_metaballs_;
  TerrainNoiseFbmGrid noise_fbm_grid_;
  TerrainNoiseFbmMulti noise_fbm_multi_;
  TerrainNoiseFbmdPerlin noise_fbmd_perlin_;
  TerrainNoiseFbmWarp noise_fbm_warp_;
  TerrainNoiseFbmPerlinWarp noise_fmb_perlin_warp_;
  std::array<ITerrainNoise*, 8> noises_;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H
