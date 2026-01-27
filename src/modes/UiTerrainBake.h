#ifndef WIREBOUNDWORLDCREATOR_UITERRAINBAKE_H
#define WIREBOUNDWORLDCREATOR_UITERRAINBAKE_H

#include <glad/glad.h>

#include "../core/Tile.h"
#include "../core/UiComplex.h"

class UiTerrainBake final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTerrainBake(Tile& cur_tile, UiSprite&& sprite, float size_scale,
                UiToggle2&& pin, UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                UiSprite&& accept, UiTextModeId&& erosion_label,
                UiTextInput&& erosion_input, UiTextModeId&& weathering_label,
                UiTextInput&& weathering_input);

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

  UiSprite accept_;

  //  UiTextInput erosion_input_;
  //  UiTextInput weathering_input_;
  UiTextModeId erosion_label_;
  UiTextInput erosion_input_;
  UiTextModeId weathering_label_;
  UiTextInput weathering_input_;

  // TODO: bind from TileRenderer, same with UiEditTerrain
  UiSprite sprite_hmap_;

  UiEventHandler ui_event_handler_;
  UiHierarchy hierarchy_;

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
};

#endif  // WIREBOUNDWORLDCREATOR_UITERRAINBAKE_H
