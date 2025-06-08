#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_

#include "../common/Shader.h"
#include "../common/Texture.h"

class UiTerrainGenerator {
 public:
  UiTerrainGenerator()
      : shader_gen_hmap_("../shaders/GenTerrainHMap.comp") {}

  //TODO: generation for height map, occlusion, erosion, etc...

  void GenerateHmap(const Texture& texture_nmap,
                    const Texture& texture_terrain_height) {
    shader_gen_hmap_.Bind();
    glBindImageTexture(
        0, texture_terrain_height.GetId(), 0,
        GL_FALSE, 0, GL_READ_ONLY, texture_terrain_height.GetFormat());
    glBindImageTexture(
        1, texture_nmap.GetId(), 0,
        GL_FALSE, 0, GL_WRITE_ONLY, texture_nmap.GetFormat());
    glDispatchCompute(1024 / 8, 1024 / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    texture_nmap.Store("d.png", 2, GL_RG, GL_UNSIGNED_BYTE);
  }

 private:
  Shader shader_gen_hmap_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
