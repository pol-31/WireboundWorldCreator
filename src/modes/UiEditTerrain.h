#ifndef WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H
#define WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "../common/TextRenderer.h"
#include "../core/Tile.h"
#include "../core/UiConfigWindow.h"
#include "TerrainNoiseData.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"
#include "UiTerrainConfig.h"
#include "traits/TerrainTraits.h"

class UiEditTerrain final : public IUiEdit {
 public:
  UiEditTerrain(Tile& cur_tile, UiSharedResources& ui_shared_resources,
                TextRenderer& text_renderer, UiEditSlots& ui_edit_slots,
                UiEditConfigSlCfg& value_config,
                UiConfigWindow& ui_noise_config);

  ~UiEditTerrain() override { DeInit(); }

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  void RemoveInstance(GLuint id) override;

  void Reset() override;

  void Generate() override;

  void GenerateAll() override;

  void RandomGenerate() override;

  bool Press(int id, float height) override;

  void Release() override;

  void Render(float height) override;

  void RenderPicking(float height) override;

  std::vector<TerrainTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] TerrainTraits& GetInstanceData() noexcept;

  void RenderGraph();

  void SetPivotPosition(GLuint pressed_id);

 private:
  static int CalculateGradientId(const glm::vec3& rotation);

  void Init();

  void DeInit();

  glm::vec4 GetLayerCentre();

  [[nodiscard]] bool IsHmapNan() const noexcept;

  void MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                   const NoiseDataBase* noise, ITerrainNoise* noise_gen);

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

  TerrainNoisePerlin gen_perlin_;
  TerrainNoiseCellular gen_cellular_;
  TerrainNoiseMetaballs gen_metaballs_;
  TerrainNoiseFbmGrid gen_fbm_grid_;
  TerrainNoiseFbmMulti gen_fbm_multi_;
  TerrainNoiseFbmdPerlin gen_fbmd_perlin_;
  TerrainNoiseFbmWarp gen_fbm_warp_;
  TerrainNoiseFbmPerlinWarp gen_fbm_perlin_warp_;
  std::array<ITerrainNoise*, 8> noises_;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITTERRAIN_H
