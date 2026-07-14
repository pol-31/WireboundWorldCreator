#include "TerrainRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "../io/Window.h"
#include "../common/Material.h"
#include "Layers.h"

TerrainRenderer::TerrainRenderer(const MaterialArray* material)
    : material_(material) {
  //TODO: Initialize()... here
}

Texture CreateTexture(float terrain_size, const std::vector<float>& height_data) {
  // 1. Generate and bind the OpenGL texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // 2. Set texture parameters (essential for the texture to render correctly)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // 3. Upload the float height data
  glTexImage2D(
      GL_TEXTURE_2D,
      0,                          // Mipmap level
      GL_R32F,                    // Internal format (32-bit float, Red/Single channel)
      terrain_size,               // Width
      terrain_size,               // Height
      0,                          // Border (must be 0)
      GL_RED,                     // Format of the pixel data
      GL_FLOAT,                   // Data type of the pixel data
      height_data.data()          // Pointer to the raw float data
  );

  // Unbind the texture to clean up the state
  glBindTexture(GL_TEXTURE_2D, 0);

  // 4. Instantiate your custom Texture class
  Texture32F terrain_texture(
      texture_id,                 // GLuint id
      terrain_size,               // GLsizei width
      terrain_size,               // GLsizei height
      GL_RED,                     // GLenum format
      GL_R32F,                    // GLint format_internal
      GL_FLOAT                    // GLenum type
  );
  return terrain_texture;
}

void TerrainRenderer::InitializeBody(JPH::BodyInterface* body_interface) {
  const int terrain_size = 1024;
  std::vector<float> height_data(terrain_size * terrain_size, 0.0f);
  hmap_ = CreateTexture(terrain_size, height_data);
  JPH::Vec3 offset(-512.0f, 0.0f, -512.0f);
  JPH::Vec3 scale(1.0f, 1.0f, 1.0f);
  JPH::HeightFieldShapeSettings shape_settings(
      height_data.data(),
      offset,
      scale,
      terrain_size
  );
  shape_settings.mActiveEdgeCosThresholdAngle = 0.996195f; // ~5 degrees
  JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
  if (shape_result.HasError()) {
    throw std::runtime_error("unable to create hmap floor");
  }
  JPH::ShapeRefC local_shape = shape_result.Get();
  JPH::Vec3 translation(0.0f, 0.0f, 0.0f);
  JPH::Quat rotation = JPH::Quat::sIdentity();
  JPH::EMotionType motion_type = JPH::EMotionType::Static;
  JPH::ObjectLayer object_layer = Layers::NON_MOVING/* Your non-moving layer, e.g., Layers::NON_MOVING */;
  JPH::BodyCreationSettings obj_settings(
      local_shape,
      translation,
      rotation,
      motion_type,
      object_layer
  );
  JPH::EActivation activation_state = JPH::EActivation::DontActivate;
  body_id_ = body_interface->CreateAndAddBody(obj_settings, activation_state);
  // node->shape = local_shape;
  //static_objects_.emplace_back(obj);
}

TerrainRenderData TerrainRenderer::GetRenderData() const noexcept {
  int material_id = 0;
  return {mesh_.GetVao(), GeoClipmaps::gPatchNum,
    hmap_.GetId(), material_id};
}
