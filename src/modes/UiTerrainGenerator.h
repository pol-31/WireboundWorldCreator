#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_

#include "../common/Shader.h"
#include "../common/Texture.h"

#include "../core/Ui.h"
#include "../core/UiComplex.h"

#include "../core/Tile.h"

class UiTerrainGenerator final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTerrainGenerator(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      Tile& cur_tile
      /*UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,

      UiStaticSprite&& btn_close,
      UiStaticSprite&& btn_accept,
      UiStaticSprite&& btn_decline,

      UiDynamicSprite&& cross*/);

  ~UiTerrainGenerator() {
    DeInit();
  }

//  UiTerrainGenerator(UiTerrainGenerator&& other) noexcept;
  UiTerrainGenerator(UiTerrainGenerator&& other) = delete;
  UiTerrainGenerator(const UiTerrainGenerator& other) = delete;

  UiTerrainGenerator& operator=(UiTerrainGenerator&& other) = delete;
  UiTerrainGenerator& operator=(const UiTerrainGenerator& other) = delete;

  // returns "stop render"
  bool Render(bool show) {
    return false;
  }

  bool Render() override {
    return false;
  }

  void RenderPicking() override{
    //
  }

  bool Press(int id) override {
//    ui_event_handler_.Press(id);
  }

  void Release() override {
//    ui_event_handler_.Release();
  }

  data::TextId Hover(int id) override {
    return data::TextId::kNone;
//    return ui_event_handler_.Hover(id);
  }

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override {
    //
  }

  void Bake();

  static const int gTerrainWidth;
  static const int gTerrainHeight;

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

  void Init();

  void DeInit();

  Texture HmapFromNoise(const Shader& shader,
                        std::vector<unsigned char>& buffer,
                        std::string_view tex_name);

  void UpdateCpuData();

  //  UiStaticSprite btn_close_;
//  UiStaticSprite btn_accept_;
//  UiStaticSprite btn_decline_;
//
  // 3 buttons
//  UiEventHandler<3> ui_event_handler_;
//
//  UiDynamicSprite cross_;


  // --------------
  // --------------
  // --------------
  // --------------

  void GenerateHmap();

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

  std::vector<std::vector<float>> height_map_data_;

  Shader shader_gen_nmap_;
  Shader shader_gen_slope_map_;
  Shader shader_gen_ao_map_;
  Shader shader_perturbate_;

 public:
  Texture& tex_hmap_;
  Texture& tex_nmap_;
  Texture& tex_slope_map_;
  Texture& tex_ao_map_;
  Texture& tex_splat_map_;

  Texture& tex_erosion_thermal_map_;
  Texture& tex_erosion_hydraulic_map_;

  Texture& tex_water_accum_;
  Texture& tex_water_flow_;

 private:
  GLuint vao_id_;
  GLuint vbo_id_;
  GLuint ebo_id_;
  GLuint fbo_id_;
  GLuint fbo_tex_id_;

  Shader shader_terrain_cellular_;
  Shader shader_terrain_fbmd_perlin_;
  Shader shader_terrain_fbm_grid_;
  Shader shader_terrain_fbm_multi_;
  Shader shader_terrain_fbm_perlin_warp_;
  Shader shader_terrain_fbm_warp_;
  Shader shader_terrain_metaballs_;
  Shader shader_terrain_perlin_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
