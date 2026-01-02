#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_

#include <random>

#include "../core/Tile.h"
#include "../core/UiComplex.h"
#include "../core/UiConfigWindow.h"
#include "IUiEdit.h"
#include "TerrainInstanceData.h"
#include "UiEditShared.h"
#include "UiTerrainConfig.h"

class UiEditTerrain final : public IUiEdit {
 public:
  using Base = IUiEdit;

  UiEditTerrain(Tile& cur_tile,
                UiSharedResources& ui_shared_resources,  // c
                WindowQueue& window_queue,               // w
                TextRenderer& text_renderer,
                std::vector<BaseInstanceData>& base_instances,
                const int& selected_id, UiConfigWindow& ui_noise_config);

  ~UiEditTerrain() { DeInit(); }

  UiEditTerrain(UiEditTerrain&& other) noexcept;
  UiEditTerrain(const UiEditTerrain& other) = delete;

  UiEditTerrain& operator=(UiEditTerrain&& other) = delete;
  UiEditTerrain& operator=(const UiEditTerrain& other) = delete;

  void HideAll() override;

  void CreateInstance() override;

  /// update base hmap (after modifying)
  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  void RemoveInstance(GLuint id) override;

  NoiseTerrainData Generate();

  void RandomGenerate();

  void SetPivotPosition(GLuint pressed_id);

  [[nodiscard]] TerrainInstanceData& GetInstanceData() noexcept {
    return instances_[selected_id_];
  }

  void RenderGraph();

  void Reset() override;

 private:
  void Init();

  void DeInit();

  void MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                   const NoiseDataBase* noise);

  static int CalculateGradientId(const glm::vec3& rotation);

  void RenderNoiseConfig();

  void RenderPickingNoiseConfig();

  glm::vec4 GetLayerCentre();

  [[nodiscard]] bool IsHmapNan() const noexcept;

  std::vector<TerrainInstanceData> instances_;
  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;

  std::mt19937 random_generator_;

  GLuint ssbo_atomic_modified_;
  glm::vec4 pivot_offset_ = glm::vec4(0.0f);
  Shader shader_merge_noises_;
  Shader shader_flatten_prep_;
  Shader shader_flatten_step_;
  Shader shader_flatten_merge_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;
  UiDynamicSprite random_generate_;
  UiTextModeId text_noise_invert_;
  UiTextModeId text_noise_tiling_;
  UiTextModeId text_noise_strength_;

  UiEditConfigTerrain noise_layer_config_;
  UiConfigWindow& ui_noise_config_;

  TerrainNoisePerlin noise_perlin_;
  TerrainNoiseCellular noise_cellular_;
  TerrainNoiseMetaballs noise_metaballs_;
  TerrainNoiseFbmGrid noise_fbm_grid_;
  TerrainNoiseFbmMulti noise_fbm_multi_;
  TerrainNoiseFbmdPerlin noise_fbmd_perlin_;
  TerrainNoiseFbmWarp noise_fbm_warp_;
  TerrainNoiseFbmPerlinWarp noise_fmb_perlin_warp_;
  std::array<ITerrainNoise*, 8> noises_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kTerrainEditNoiseHmap) -
                 static_cast<int>(data::VboIdMain::kTerrainEditDesk) + 1>
      ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

class UiTerrainBake final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTerrainBake(Tile& cur_tile, UiDynamicSprite&& sprite, float size_scale,
                UiToggle2&& pin, UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                UiDynamicSprite&& accept, UiTextModeId&& erosion_label,
                UiTextInput&& erosion_input, UiTextModeId&& weathering_label,
                UiTextInput&& weathering_input);

  UiTerrainBake(UiTerrainBake&& other) noexcept;
  UiTerrainBake(const UiTerrainBake& other) = delete;

  UiTerrainBake& operator=(UiTerrainBake&& other) = delete;
  UiTerrainBake& operator=(const UiTerrainBake& other) = delete;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  void Bake(int steps_thermal, int steps_weathering, float talus);

 private:
  struct Particle {
    // Construct Particle at Position
    Particle(glm::vec2 _pos) { pos = _pos; }

    glm::vec2 pos;
    glm::vec2 speed = glm::vec2(0.0);

    float volume = 1.0;    // This will vary in time
    float sediment = 0.0;  // Fraction of Volume that is Sediment!
  };

  void UpdateCpuData();

  void ErodeWithFlow(const std::vector<std::vector<glm::vec2>>& flow_dir,
                     const std::vector<std::vector<float>>& water_accum,
                     int cycles);

  glm::vec3 SurfaceNormal(int i, int j);

  void ErodeWeathering(int cycles);

  void ProcessErosion(const std::vector<std::vector<glm::vec2>>& flow_dir,
                      const std::vector<std::vector<float>>& water_accum,
                      int iterations);

  void ProcessThermalWeathering(int iterations, float talus = 0.02f);

  void GenerateSplatmap();

  void GenerateNmap();

  void GenerateSlope();

  void GenerateAo();

  void Perturbate();

  void ComputeFlowMaps(std::vector<std::vector<glm::vec2>>& flow_dir,
                       std::vector<std::vector<float>>& flow_accum);

  void GenerateFlowMap(std::vector<std::vector<glm::vec2>>& flow_dir,
                       std::vector<std::vector<float>>& flow_accum);

  UiDynamicSprite accept_;

  //  UiTextInput erosion_input_;
  //  UiTextInput weathering_input_;
  UiTextModeId erosion_label_;
  UiTextInput erosion_input_;
  UiTextModeId weathering_label_;
  UiTextInput weathering_input_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kTerrainBakeHmap) -
                 static_cast<int>(data::VboIdMain::kTerrainBakeDesk) + 1>
      ui_event_handler_;

  UiSharedResources& ui_shared_resources_;

  Texture32F& tex_hmap_;
  std::vector<GLfloat>& hmap_heights_;

  Shader shader_gen_nmap_;
  Shader shader_gen_slope_map_;
  Shader shader_gen_splat_map_;
  Shader shader_gen_ao_map_;
  Shader shader_perturbate_;

  Texture& tex_nmap_;
  Texture& tex_slope_map_;
  Texture& tex_ao_map_;
  Texture& tex_splat_map_;

  Texture32F& tex_erosion_thermal_map_;
  Texture32F& tex_erosion_hydraulic_map_;
  Texture32F& tex_water_accum_;
  Texture& tex_water_flow_;

  // TODO: bind from TileRenderer, same with UiEditTerrain
  UiDynamicSprite sprite_hmap_;
  /*UiDynamicSprite sprite_nmap_;
  UiDynamicSprite sprite_slopemap_;
  UiDynamicSprite sprite_ao_;
  UiDynamicSprite sprite_splatmap_;
  UiDynamicSprite sprite_erosion_thermal_;
  UiDynamicSprite sprite_erosion_hydraulic_;
  UiDynamicSprite sprite_water_accum_;
  UiDynamicSprite sprite_water_flow_;*/
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_
