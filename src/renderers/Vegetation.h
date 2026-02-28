#ifndef WIREBOUNDWORLDCREATOR_VEGETATION_H
#define WIREBOUNDWORLDCREATOR_VEGETATION_H

#include <glad/glad.h>

#include "../render/Shader.h"
#include "../render/Texture.h"

struct GeoSurface;

class Vegetation {
 public:
  Vegetation();

  ~Vegetation() { DeInit(); }

  void Render(float map_scale, const Texture& hmap);

  /// every frame(!) we generate grass blades and discard wrt placement_map
  void Update(float map_scale, const GeoSurface* surface);

  /// density: 1.0f - every pixel, 0.0f - never
  /// seed: model / instance / tile id
  /// mask - got after rivers & roads, modified for next stages
  std::vector<GLuint> UpdatePipeline(std::vector<uint8_t>& mask,
                                     Texture& placement, float density,
                                     uint32_t seed);

  /// MurmurHash3 finalizer
  float PlacementHash(uint32_t x);

 private:
  void Init();

  void DeInit();

  /// UpdatePipeline here, get its data and collect points
  Texture placement_temp_;

  Shader grass_shader_;
  Shader grass_compute_shader_;

  GLuint vao_ = 0;
  GLuint ssbo_ = 0;
  GLsizei blades_num_{0};
  Texture tex_grass_;
};

#endif  // WIREBOUNDWORLDCREATOR_VEGETATION_H
