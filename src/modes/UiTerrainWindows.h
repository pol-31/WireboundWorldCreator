#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_

#include <random>

#include "../core/UiComplex.h"
#include "../core/Tile.h"

#include "UiTerrainConfig.h"
#include "TerrainInstanceData.h"

// no parent-child rel; facade pattern
class UiNoiseLayerConfig {
 public:
  UiNoiseLayerConfig(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& config, UiTextModeId&& text_name,
      UiToggle4&& toggle_invert, UiToggle4&& toggle_tiling,
      UiSliderH2&& slider_strength,
      UiDynamicSprite&& hmap);

  UiNoiseLayerConfig(UiNoiseLayerConfig&& other) noexcept = default;
  UiNoiseLayerConfig(const UiNoiseLayerConfig& other) = delete;

  UiNoiseLayerConfig& operator=(UiNoiseLayerConfig&& other) = delete;
  UiNoiseLayerConfig& operator=(const UiNoiseLayerConfig& other) = delete;

  /// so we could get GetTopBorder & GetBottomBorder and estimate position
  void ResetTransform();

  /// no Press(), Release() <- done in external ui_event_handler
  void Render(NoiseDataBase* terrain_data, glm::vec2 translate,
              bool update_strength, data::TextId text_id);

  void RenderPicking(glm::vec2 translate);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  // public, for simpler external ui_event_handler adding
  UiDynamicSprite config_;
  UiTextModeId text_name_;
  UiToggle4 toggle_invert_;
  UiToggle4 toggle_tiling_;
  UiSliderH2 slider_strength_;
  UiDynamicSprite hmap_;

  UiSharedResources& ui_shared_resources_;
};

class UiEditBase : public UiWindowAppear {
 public:
  using UiWindowAppear::UiWindowAppear;
  virtual void UpdateConfig() = 0; // e.g. UpdateHmap()
  virtual void SetInstanceId(int id) = 0; // insctances_[id]
  virtual void HideAll() = 0;
  virtual void CreateInstance() = 0;
  virtual void Reset() = 0;
};

class UiEditObjects : public UiEditBase {
 public:
  using Base = UiEditBase;
  UiEditObjects(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue);

  void UpdateConfig() override {}
  void SetInstanceId(int id) override {}
  void HideAll() override {}
  void CreateInstance() override {}

  bool Press(int id) override {return false;}
  void Release() override {}
  bool Scroll(GLuint id, float yoffset) override {return false;}
  bool Render() override {return false;}
  void RenderPicking() override {}

  void Reset() override {}
};

class UiEditBiomes : public UiEditBase {
 public:
  using Base = UiEditBase;
  UiEditObjects(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue);

  void UpdateConfig() override {}
  void SetInstanceId(int id) override {}
  void HideAll() override {}
  void CreateInstance() override {}

  bool Press(int id) override {return false;}
  void Release() override {}
  bool Scroll(GLuint id, float yoffset) override {return false;}
  bool Render() override {return false;}
  void RenderPicking() override {}

  void Reset() override {}

  UiDynamicSprite btn_wind_;
  UiDynamicSprite btn_sun_;
  UiDynamicSprite btn_time_;
  UiDynamicSprite btn_precipitations_;
  UiDynamicSprite btn_temperature_;
  UiDynamicSprite btn_clouds_;
};

class UiEditTerrain final : public UiEditBase {
 public:
  using Base = UiEditBase;

  UiEditTerrain(
      Tile& cur_tile,
      UiSharedResources& ui_shared_resources, // c
      WindowQueue& window_queue, // w
      TextRenderer& text_renderer,
      std::vector<BaseInstanceData>& base_instances,
      const int& selected_id);

  ~UiEditTerrain() {
    DeInit();
  }

  UiEditTerrain(UiEditTerrain&& other) noexcept;
  UiEditTerrain(const UiEditTerrain& other) = delete;

  UiEditTerrain& operator=(UiEditTerrain&& other) = delete;
  UiEditTerrain& operator=(const UiEditTerrain& other) = delete;

  void HideAll() override;

  void CreateInstance() override;

  /// update base hmap (after modifying)
  void UpdateConfig() override;

  void SetInstanceId(int id) override {
    SetTerrainData(id);
  }

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  // can't use Render(terrain_data) because of must-have virtual-ness
  void SetTerrainData(int id);

  void RenderPicking() override;

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
                   NoiseDataBase* noise_data, const NoiseDataBase* noise);

  static int CalculateGradientId(const glm::vec3& rotation);

  void RenderNoiseConfig();

  void RenderPickingNoiseConfig();

  int GetSliderNoiseId(glm::vec2 mouse_pos);

  float GetEntryHeight();

  glm::vec4 GetLayerCentre();

  //TODO: integrate with UiSlotsTerrain::instances_
  std::vector<TerrainInstanceData> instances_;

  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;


  Texture32F& tex_hmap_;
  std::vector<GLfloat>& hmap_heights_;

  GLuint vao_id_;
  GLuint vbo_id_;
  GLuint ebo_id_;
  GLuint fbo_id_;
  GLuint fbo_tex_id_;

  GLuint ssbo_atomic_modified_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  //  UiDynamicSprite name_;
  //  UiDynamicSprite name_back_;

  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;

  UiDynamicSprite random_generate_;

  UiTextModeId text_noise_invert_;
  UiTextModeId text_noise_tiling_;
  UiTextModeId text_noise_strength_;

  UiNoiseLayerConfig noise_layer_config_; // noise1-8
  // all needed data stored in vector instances_[]

  TerrainNoisePerlin noise_perlin_;
  TerrainNoiseCellular noise_cellular_;
  TerrainNoiseMetaballs noise_metaballs_;
  TerrainNoiseFbmGrid noise_fbm_grid_;
  TerrainNoiseFbmMulti noise_fbm_multi_;
  TerrainNoiseFbmdPerlin noise_fbmd_perlin_;
  TerrainNoiseFbmWarp noise_fbm_warp_;
  TerrainNoiseFbmPerlinWarp noise_fmb_perlin_warp_;
  std::array<ITerrainNoise*, 8> noises_;

  UiTerrainNoise ui_terrain_noise_;

  std::mt19937 random_generator_;

  Shader shader_merge_noises_; /// 7 times merging (8 noises)

  /// generate hmap from transformed layers
  Shader shader_flatten_prep_;
  Shader shader_flatten_step_;
  Shader shader_flatten_merge_;

  Texture32F tex_mesh_; /// temp for base hmap baking

  //  ITerrainNoise* selected_noise_ = nullptr;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainWindowNoiseSliderIcon) -
      static_cast<int>(data::VboIdMain::kTerrainEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;

  int pressed_strength_id_ = -1.0f;

  glm::vec4 pivot_offset_ = glm::vec4(0.0f);
};

class UiTerrainBake final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTerrainBake(
      Tile& cur_tile,
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
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
  struct Particle{
    //Construct Particle at Position
    Particle(glm::vec2 _pos) {
      pos = _pos;
    }

    glm::vec2 pos;
    glm::vec2 speed = glm::vec2(0.0);

    float volume = 1.0;   //This will vary in time
    float sediment = 0.0; //Fraction of Volume that is Sediment!
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

  void ComputeFlowMaps(
      std::vector<std::vector<glm::vec2>>& flow_dir,
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

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainBakeHmap) -
      static_cast<int>(data::VboIdMain::kTerrainBakeDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;


  Texture32F& tex_hmap_;
  std::vector<GLfloat>& hmap_heights_;

  Shader shader_gen_nmap_;
  Shader shader_gen_slope_map_;
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

  //TODO: bind from TileRenderer, same with UiEditTerrain
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
