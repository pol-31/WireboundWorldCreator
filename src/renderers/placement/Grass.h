#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENT_GRASS_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENT_GRASS_H_

#include <chrono>
#include <string_view>

#include <glad/glad.h>

#include "../../common/Shader.h"
#include "../../common/Texture.h"
#include "../../common/Paths.h"
#include "../../core/Tile.h"

class Grass {
 public:
  explicit Grass(const Paths& paths);

  ~Grass() {
    DeleteBuffers();
  }

  //  using DeltaDuration = std::chrono::duration<float, std::milli>;

  //TODO: because both Update() and Render() and solely GPU tasks,
  // there's no benefit to separate rendering into two calls:
  // anyway we will do this each frame. SO:
  //TODO: REMOVE compute shaders (bear functional into vertex/tessellation shader.
  // separate Update() and Render() useful only for CPU+GPU task

   void UpdateAnimation(float delta_time);

   void Render();

  /// when new tile was chosen or modified current (if it affects grass
  /// placement) we should call it to generate blades with new positions
  void UpdatePlacement(const std::vector<int>& points,
                       const std::vector<uint8_t>& heights);

  void SetWindMagnitude(float wind_magnitude);
  void SetWindWaveLength(float wind_wave_length);
  void SetWindWavePeriod(float wind_wave_period);

 private:
  // Indirect drawing structure (TODO: cur not used)
  struct NumBlades {
    std::uint32_t vertexCount = 5;
    std::uint32_t instanceCount = 1;
    std::uint32_t firstVertex = 0;
    std::uint32_t firstInstance = 0;
  };

  struct Blade {
    glm::vec4 v0; // xyz: Position, w: orientation (in radius)
    glm::vec4 v1; // xyz: Bezier point w: height
    glm::vec4 v2; // xyz: Physical model guide w: width
    glm::vec4 up; // xyz: Up vector w: stiffness coefficient
  };

  void Init();
  void DeleteBuffers() const;

  static std::vector<Blade> GenerateBlades(
      const std::vector<int>& points,
      const std::vector<uint8_t>& heights);

  GLuint grass_vao_{0};
  Shader grass_shader_;
  Shader grass_compute_shader_;
  GLsizei blades_num_{0};

  GLuint grass_input_buffer_{0};
  GLuint grass_output_buffer_{0};
  GLuint grass_indirect_buffer_{0};

  // Wind parameters // TODO:
  float wind_magnitude_ = 100.0f;
  float wind_wave_length_ = 20.0f;
  float wind_wave_period_ = 2.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENT_GRASS_H_
