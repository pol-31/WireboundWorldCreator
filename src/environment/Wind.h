#ifndef WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_WIND_H_
#define WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_WIND_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../common/Paths.h"
#include "../common/Texture.h"

/// Simple scrolling of a Perlin noise texture,
/// adjusting its velocity (speed + direction)
/*
   vec2 offset = velocity * time;
   vec2 scrolledTexCoords = texCoords + offset;
   fragColor = texture(noiseTexture, scrolledTexCoords);
 * */
class Wind {
 public:
  Wind(const Paths& paths, GLuint ubo_id);

  void Update(float direction, float speed);

  [[nodiscard]] float GetSpeed() const;
  [[nodiscard]] float GetDirection() const;

 private:
  void Init();

  Texture texture_;
  glm::vec2 velocity_;
  GLuint ubo_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_WIND_H_
