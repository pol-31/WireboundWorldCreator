#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_

#include "../common/Shader.h"
#include "../common/Texture.h"

class UiTerrainGenerator {
 public:
  UiTerrainGenerator()
      : shader_gen_hmap_("../shaders/GenTerrainHMap.comp") {}

  //TODO: generation for height map, occlusion, erosion, etc...

  /*void GenerateHmap(const Texture& texture_nmap,
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
  }*/


  void ErodeWithFlow(std::vector<std::vector<float>>& heightmap,
                     const std::vector<std::vector<Vec2>>& flow_dir,
                     const std::vector<std::vector<float>>& water_accum,
                     int cycles);


  Vec3 surfaceNormal(std::vector<std::vector<float>>& heightmap, int i, int j);

  void ErodeWeathering(std::vector<std::vector<float>>& heightmap, int cycles);


  Texture ProcessErosion(std::vector<std::vector<float>>& height_map_data,
                         const std::vector<std::vector<Vec2>>& flow_dir,
                         const std::vector<std::vector<float>>& water_accum,
                         int iterations);

  Texture ProcessThermalWeathering(
      std::vector<std::vector<float>>& height_map_data, int iterations,
      float talus = 0.02f);

  Texture GenerateSplatmap();

  Texture GenerateNmap(
      const Shader& shader_gen_nmap,
      const Texture& tex_hmap);

  Texture GenerateSlope(
      const Shader& shader_gen_slope_map,
      const Texture& tex_hmap);

  Texture GenerateAo(
      const Shader& shader_gen_ao_map,
      const Texture& tex_hmap);

  Texture Perturbate(
      const Shader& shader_perturbate,
      const Texture& tex_hmap);

  void ComputeFlowMaps(
      std::vector<std::vector<float>> height_map_data,
      std::vector<std::vector<Vec2>>& flow_dir,
      std::vector<std::vector<float>>& flow_accum);

  Texture GenerateFlowMap(std::vector<std::vector<float>>& height_map_data,
                          std::vector<std::vector<Vec2>>& flow_dir,
                          std::vector<std::vector<float>>& flow_accum);

  void Bake(std::vector<unsigned char>& height_map_data,
            int width, int height, Texture& height_map);

 private:
  struct Vec2 {
    float x;
    float y;

    // Compute the Euclidean length of the vector
    static float length(const Vec2& v) {
      return std::sqrt(v.x * v.x + v.y * v.y);
    }

    // Return a normalized (unit-length) version of the vector
    static Vec2 normalize(const Vec2& v) {
      float len = length(v);
      if (len > 1e-6f) {
        return { v.x / len, v.y / len};
      } else {
        return { 0.0f, 0.0f }; // return zero vector if length is too small
      }
    }
  };
  struct Vec2i {
    int x;
    int y;
  };
  struct Vec3 {
    float x;
    float y;
    float z;

    // Compute the Euclidean length of the vector
    static float length(const Vec3& v) {
      return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    // Return a normalized (unit-length) version of the vector
    static Vec3 normalize(const Vec3& v) {
      float len = length(v);
      if (len > 1e-6f) {
        return { v.x / len, v.y / len, v.z / len };
      } else {
        return { 0.0f, 0.0f, 0.0f }; // return zero vector if length is too small
      }
    }
    Vec3 operator*(float m) const {
      return {x * m,
              y * m,
              z * m};
    }

    // scalar * Vec3 (global function)
    void operator+=(const Vec3& v) {
      x += v.x;
      y += v.y;
      z += v.z;
    }
  };

  struct Particle{
    //Construct Particle at Position
    Particle(Vec2 _pos){ pos = _pos; }

    Vec2 pos;
    Vec2 speed = Vec2(0.0);

    float volume = 1.0;   //This will vary in time
    float sediment = 0.0; //Fraction of Volume that is Sediment!
  };

  Shader shader_gen_hmap_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINGENERATOR_H_
