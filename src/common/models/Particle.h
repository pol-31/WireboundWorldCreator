#ifndef WIREBOUNDWORLDCREATOR_PARTICLE_H
#define WIREBOUNDWORLDCREATOR_PARTICLE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "../../render/Shader.h"
#include "../../render/SpriteUv.h"

struct ParticleGpu {
  glm::mat4 modelMatrix;
  glm::vec4 color;
  float lifetime;
  float size;
  uint32_t materialId;
  uint32_t padding[3];
};

struct Particle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  float lifetime;
  float maxLifetime;
  float size;
  glm::vec4 color;
  uint32_t materialId;
  bool alive;
};

struct ParticleSourceConfig {
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 spread;
  float speedMin = 1.0f;
  float speedMax = 5.0f;
  float lifetimeMin = 0.5f;
  float lifetimeMax = 1.0f;
  float sizeMin = 0.1f;
  float sizeMax = 0.3f;
  glm::vec4 colorMin = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec4 colorMax = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float emissionRate = 10.0f; // Particles per second
  float gravity = -9.81f;
  uint32_t materialId = 0;
  bool active = true;
  bool oneShot = false;
  int burstCount = 10;
  float emissionAccumulators_;
  bool deleted = false; // as a pool member
};

class ParticleManager {
 public:
  ParticleManager(GLuint tex_id, SpriteUv uv);

  ~ParticleManager() {
    DeInit();
  }

  void Render();

  void Update(float deltaTime);

  void Emit(const ParticleSourceConfig& config);

  int AddSource(const ParticleSourceConfig& config);
  
  void RemoveSource(int id);

  ParticleSourceConfig& GetSource(int id) {
    return sources_[id];
  }

  void UploadToGPU();

  GLuint GetVAO() const { return vao_; }
  GLuint GetSSBO() const { return ssbo_; }
  int GetActiveParticleCount() const { return activeCount_; }

  static const int sMaxParticles;
  static const int sMaxSources;

 private:
  void Init(SpriteUv uv);

  void DeInit();

  int AllocateParticle();
  
  void FreeParticle(int index);

  std::vector<Particle> particles_;
  int activeCount_ = 0;
  GLuint tex_id_ = 0;
  Shader sh_particle_;

  std::vector<ParticleSourceConfig> sources_;

  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  GLuint ebo_ = 0;
  GLuint ssbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_PARTICLE_H
