#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_

#include "../core/UiComplex.h"
#include "../core/Tile.h"

#include "UiTerrainConfig.h"
#include "TerrainInstanceData.h"

// no parent-child rel; facade pattern
class UiEditTerrainNoise {
 public:
  UiEditTerrainNoise(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& config,
      UiTextLabelId&& text_name,
      UiToggle&& toggle_invert,
      UiToggle&& toggle_tiling,
      UiSliderH2&& slider_strength,
      UiDynamicSprite&& hmap);

  UiEditTerrainNoise(UiEditTerrainNoise&& other) noexcept = default;
  UiEditTerrainNoise(const UiEditTerrainNoise& other) = delete;

  UiEditTerrainNoise& operator=(UiEditTerrainNoise&& other) = delete;
  UiEditTerrainNoise& operator=(const UiEditTerrainNoise& other) = delete;

  /// no Press(), Release(), Hover() <- done in external ui_event_handler
  void Render(const Texture32F& hmap_ref);

  void RenderPicking();

  void SetParentId(int id);

  void UpdateTransform();

  void SetConfig(NoiseDataBase* terrain_data);

  // public, for simpler external ui_event_handler adding
  UiDynamicSprite config_;
  UiTextLabelId text_name_;
  UiToggle toggle_invert_;
  UiToggle toggle_tiling_;
  UiSliderH2 slider_strength_;
  UiDynamicSprite hmap_;

  UiSharedResources& ui_shared_resources_;
};

class UiEditTerrain final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiEditTerrain(
      Tile& cur_tile,
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      UiDynamicSprite&& accept,
      UiDynamicSprite&& name,
      UiDynamicSprite&& name_back,
      UiSlider2D&& color_palette,
      UiSliderH2&& color_brightness,
      UiDynamicSprite&& color_indicator,
      UiDynamicSprite&& random_generate,
      const std::vector<TerrainInstanceData>& instances,
      const int& instances_size);

  ~UiEditTerrain() {
    DeInit();
  }

  UiEditTerrain(UiEditTerrain&& other) noexcept;
  UiEditTerrain(const UiEditTerrain& other) = delete;

  UiEditTerrain& operator=(UiEditTerrain&& other) = delete;
  UiEditTerrain& operator=(const UiEditTerrain& other) = delete;

  // update base hmap (after modifying smt)
  void UpdateHmap();

  data::TextId Hover(int id) override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  // can't use Render(terrain_data) because of must-have virtual-ness
  void SetTerrainData(NoiseTerrainData* terrain_data);

  void RenderPicking() override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  NoiseTerrainData Generate();

  void HideAll();

 private:
  void Init();

  void DeInit();

  void MergeLayers(Texture32F& bottom_layer, Texture32F& top_layer,
                   NoiseDataBase* noise_data, const UiEditTerrainNoise& noise);

  Texture32F& tex_hmap_;
  std::vector<GLfloat>& hmap_heights_;

  GLuint vao_id_;
  GLuint vbo_id_;
  GLuint ebo_id_;
  GLuint fbo_id_;
  GLuint fbo_tex_id_;

  UiDynamicSprite accept_;
  UiDynamicSprite name_;
  UiDynamicSprite name_back_;

  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;

  UiDynamicSprite random_generate_;

  UiTextLabelId text_noise_invert_;
  UiTextLabelId text_noise_tiling_;
  UiTextLabelId text_noise_strength_;

//  Texture32F tex_perlin_;
//  Texture32F tex_cellular_;
//  Texture32F tex_metaballs_;
//  Texture32F tex_fbm_grid;
//  Texture32F tex_fbm_multi_;
//  Texture32F tex_fbmd_perlin_;
//  Texture32F tex_fbm_warp;
//  Texture32F tex_fbm_perlin_warp_;

  UiEditTerrainNoise noise1_;
  UiEditTerrainNoise noise2_;
  UiEditTerrainNoise noise3_;
  UiEditTerrainNoise noise4_;
  UiEditTerrainNoise noise5_;
  UiEditTerrainNoise noise6_;
  UiEditTerrainNoise noise7_;
  UiEditTerrainNoise noise8_;

  TerrainNoisePerlin noise_perlin_;
  TerrainNoiseCellular noise_cellular_;
  TerrainNoiseMetaballs noise_metaballs_;
  TerrainNoiseFbmGrid noise_fbm_grid_;
  TerrainNoiseFbmMulti noise_fbm_multi_;
  TerrainNoiseFbmdPerlin noise_fbmd_perlin_;
  TerrainNoiseFbmWarp noise_fbm_warp_;
  TerrainNoiseFbmPerlinWarp noise_fmb_perlin_warp_;

  // we modify it here, non const
  NoiseTerrainData* terrain_data_ = nullptr;

  Shader shader_merge_;
  Shader shader_sum_;

//  ITerrainNoise* selected_noise_ = nullptr;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainEditNoise8StrengthIcon) -
      static_cast<int>(data::VboIdMain::kTerrainEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;

  const std::vector<TerrainInstanceData>& instances_;
  const int& instances_size_;
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
      UiDynamicSprite&& accept,
      UiTextLabelId&& erosion_label,
      UiDynamicSprite&& erosion_input,
      UiTextLabelId&& weathering_label,
      UiDynamicSprite&& weathering_input);

  UiTerrainBake(UiTerrainBake&& other) noexcept;
  UiTerrainBake(const UiTerrainBake& other) = delete;

  UiTerrainBake& operator=(UiTerrainBake&& other) = delete;
  UiTerrainBake& operator=(const UiTerrainBake& other) = delete;

  data::TextId Hover(int id) override;

  bool Press(int id);

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

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

  UiTextLabelId erosion_label_;
  UiDynamicSprite erosion_input_;
  UiTextLabelId weathering_label_;
  UiDynamicSprite weathering_input_;

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
  UiDynamicSprite sprite_nmap_;
  UiDynamicSprite sprite_slopemap_;
  UiDynamicSprite sprite_ao_;
  UiDynamicSprite sprite_splatmap_;
  UiDynamicSprite sprite_erosion_thermal_;
  UiDynamicSprite sprite_erosion_hydraulic_;
  UiDynamicSprite sprite_water_accum_;
  UiDynamicSprite sprite_water_flow_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINWINDOWS_H_
