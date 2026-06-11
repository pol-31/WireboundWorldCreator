#ifndef WIREBOUNDWORLDCREATOR_MATERIAL_H
#define WIREBOUNDWORLDCREATOR_MATERIAL_H

#include "../render/Texture.h"

struct Material {
  /**
   * Because of astc encoding format we always have RGBA; so to store
   * rough_metal or normal map we do (official astc-enc recommendations):
   * """
   * To encode this we need to store only two input components
   * in the compressed data, and therefore use the rrrg coding swizzle
   * to align the data with the ASTC luminance+alpha endpoint.
   * """
   * So we decided to represent material as:
   * . albedo - rgba;
   * . metallic & roughness - as rrrg coding swizzle;
   * . normal - as rrrg coding swizzle;
   * - occlusion - rrrr
   * . emission - rgba
   * TODO: occlusion & emission can be stored in the same RGBA texture
   */
  Texture albedo;
  // Texture emission;
  Texture rough_metal_ao;
  Texture normal;
};

struct MaterialArray {
  GLuint albedo = 0;
  GLuint rough_metal_ao = 0;
  GLuint normal = 0;
  uint32_t count = 0;
  // GLuint emission = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_MATERIAL_H
