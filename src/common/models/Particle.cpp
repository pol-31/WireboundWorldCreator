#include "Particle.h"

#include <random>
#include <algorithm>

const int ParticleManager::sMaxParticles = 1000;//16384;
const int ParticleManager::sMaxSources = 10;;

ParticleManager::ParticleManager(GLuint tex_id, SpriteUv uv)
  : sh_particle_("../shaders/Particle.vert", "../shaders/Particle.frag", {0}),
  tex_id_(tex_id) {
  Init(uv);
}

void ParticleManager::Init(SpriteUv uv) {
  particles_ = std::vector<Particle>(sMaxParticles);
  for (int i = 0; i < sMaxParticles; i++) {
    particles_[i].alive = false;
    particles_[i].lifetime = 0.0f;
  }
  float quadVertices[] = {
      -0.5f, -0.5f, 0.0f,  uv.min.x, uv.min.y,
       0.5f, -0.5f, 0.0f,  uv.max.x, uv.min.y,
       0.5f,  0.5f, 0.0f,  uv.max.x, uv.max.y,
      -0.5f,  0.5f, 0.0f,  uv.min.x, uv.max.y,
  };

  uint16_t quadIndices[] = {
      0, 1, 2,  2, 3, 0
  };

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
  glBindVertexArray(0);

  glGenBuffers(1, &ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sMaxParticles * sizeof(ParticleGpu), nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_);
}

void ParticleManager::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteBuffers(1, &ssbo_);
}

int ParticleManager::AllocateParticle() {
  for (int i = 0; i < sMaxParticles; i++) {
    if (!particles_[i].alive) {
      particles_[i].alive = true;
      activeCount_++;
      return i;
    }
  }
  return -1;
}

void ParticleManager::FreeParticle(int index) {
  if (index >= 0 && index < sMaxParticles && particles_[index].alive) {
    particles_[index].alive = false;
    activeCount_--;
  }
}

void ParticleManager::Render() {
  int count = GetActiveParticleCount();
  if (count == 0) return;
  UploadToGPU();
  sh_particle_.DebugUpdate();
  sh_particle_.Bind();
  glBindTextureUnit(0, tex_id_);
  glBindVertexArray(vao_);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0, count);
  glBindVertexArray(0);
}

void ParticleManager::Emit(const ParticleSourceConfig& config) {
  static thread_local std::mt19937 rng(std::hash<std::string>{}("particle_" + 
      std::to_string((size_t)this)));

  int count = config.oneShot ? config.burstCount : 1;

  for (int i = 0; i < count; i++) {
    int idx = AllocateParticle();
    if (idx < 0) break;

    Particle& p = particles_[idx];

    std::uniform_real_distribution<float> distX(-config.spread.x, config.spread.x);
    std::uniform_real_distribution<float> distY(-config.spread.y, config.spread.y);
    std::uniform_real_distribution<float> distZ(-config.spread.z, config.spread.z);

    p.position = config.position + glm::vec3(distX(rng), distY(rng), distZ(rng));

    std::uniform_real_distribution<float> speedDist(config.speedMin, config.speedMax);
    float speed = speedDist(rng);

    glm::vec3 dir = config.direction;
    dir.x += (rand() / (float)RAND_MAX - 0.5f) * config.spread.x * 2.0f;
    dir.y += (rand() / (float)RAND_MAX - 0.5f) * config.spread.y * 2.0f;
    dir.z += (rand() / (float)RAND_MAX - 0.5f) * config.spread.z * 2.0f;
    dir = glm::normalize(dir);

    p.velocity = dir * speed;
    p.acceleration = glm::vec3(0.0f, config.gravity, 0.0f);

    std::uniform_real_distribution<float> lifeDist(config.lifetimeMin, config.lifetimeMax);
    p.maxLifetime = lifeDist(rng);
    p.lifetime = p.maxLifetime;
    std::uniform_real_distribution<float> sizeDist(config.sizeMin, config.sizeMax);
    p.size = sizeDist(rng);
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    p.color.r = glm::mix(config.colorMin.r, config.colorMax.r, colorDist(rng));
    p.color.g = glm::mix(config.colorMin.g, config.colorMax.g, colorDist(rng));
    p.color.b = glm::mix(config.colorMin.b, config.colorMax.b, colorDist(rng));
    p.color.a = glm::mix(config.colorMin.a, config.colorMax.a, colorDist(rng));

    p.materialId = config.materialId;
  }
}

int ParticleManager::AddSource(const ParticleSourceConfig& config) {
  for (int i = 0; i < sources_.size(); ++i) {
    auto& s = sources_[i];
    if (s.deleted) {
      s = config;
      return i;
    }
  }
  int total_sources = static_cast<int>(sources_.size());
  if (total_sources >= sMaxSources) {
    throw std::runtime_error("ParticleManager::AddSource exceeds max num");
  }
  sources_.push_back(config);
  return total_sources;
}

void ParticleManager::RemoveSource(int id) {
  sources_[id].deleted = true;
}

void ParticleManager::Update(float deltaTime) {
  for (int i = 0; i < (int)sources_.size(); i++) {
    ParticleSourceConfig& src = sources_[i];
    if (src.deleted || !src.active) {
      continue;
    }

    if (src.oneShot) {
      src.emissionAccumulators_ = 1.0f;
      src.active = false;
    } else {
      src.emissionAccumulators_ += deltaTime * src.emissionRate;
    }
    while (src.emissionAccumulators_ >= 1.0f) {
      Emit(src);
      src.emissionAccumulators_ -= 1.0f;
    }
  }

  for (int i = 0; i < sMaxParticles; i++) {
    Particle& p = particles_[i];
    if (!p.alive) continue;
    p.lifetime -= deltaTime;
    if (p.lifetime <= 0.0f) {
      FreeParticle(i);
      continue;
    }
    p.velocity += p.acceleration * deltaTime;
    p.position += p.velocity * deltaTime;
  }
}

void ParticleManager::UploadToGPU() {
  std::vector<ParticleGpu> instances;
  instances.reserve(activeCount_);

  for (int i = 0; i < sMaxParticles; i++) {
    const Particle& p = particles_[i];
    if (!p.alive) continue;
    ParticleGpu inst;
    float lifeRatio = p.lifetime / p.maxLifetime;
    inst.color = p.color;
    inst.color.a *= lifeRatio;

    inst.modelMatrix = glm::mat4(1.0f);
    inst.modelMatrix = glm::translate(inst.modelMatrix, p.position);
    inst.modelMatrix = glm::scale(inst.modelMatrix, glm::vec3(p.size));

    inst.lifetime = p.lifetime;
    inst.size = p.size;
    inst.materialId = p.materialId;

    instances.push_back(inst);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  if (!instances.empty()) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 
                    instances.size() * sizeof(ParticleGpu), instances.data());
  }
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
