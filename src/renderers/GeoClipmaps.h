#ifndef WIREBOUNDWORLDCREATOR_GEOCLIPMAPS_H
#define WIREBOUNDWORLDCREATOR_GEOCLIPMAPS_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Camera;
struct Tile;

struct GeoMesh {
  glm::vec2 centre = glm::vec2(0.0f);
  float width = 0.0f;
  float height = 0.0f;
  float density = 1.0f;
};

struct GeoSurface {
  GeoMesh lod0;
  GeoMesh lod1;
  GeoMesh lod2;
  GeoMesh lod3;
};

class GeoClipmaps {
public:
  GeoClipmaps();

  ~GeoClipmaps() {
    DeInit();
  }

  void Update(const Camera* camera, Tile* tile);

  void BindVao() const noexcept;

  void Render();

  void RenderLowPoly() const noexcept;

  [[nodiscard]] const GeoSurface& GetSurface() const noexcept {
    return surface_;
  }

private:
  struct Patch {
    glm::vec4 p0;
    glm::vec4 p1;
    float heightLod;
    float _pad1 = 0.0f;
    float _pad2 = 0.0f;
    float _pad3 = 0.0f;
  };

  struct Band {
    float z0, z1;     // normalized [0..1] depth in trapezoid
    int columns;      // horizontal splits
    float heightLod;  // heightmap mip
  };

  struct GroundView {
    glm::vec2 nearL, nearR;
    glm::vec2 farL, farR;
  };

  void Init();

  void DeInit();

  static GroundView RaysToGround(const Camera* camera, Tile* tile);

  static const int gPatchNum;
  static const int gPatchNumLowPoly;
  GLuint vao_ = 0;
  GLuint vbo_quad_ = 0;
  GLuint ssbo_mesh_ = 0;
  GeoSurface surface_;
};

#endif  // WIREBOUNDWORLDCREATOR_GEOCLIPMAPS_H
