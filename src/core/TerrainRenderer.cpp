#include "TerrainRenderer.h"

#include <random>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <stb_image.h>

#include "../common/Material.h"
#include "../io/Window.h"
#include "Layers.h"

TerrainRenderer::TerrainRenderer(const MaterialArray* material)
    : material_(material) {
  // TODO: Initialize()... here
}

Texture CreateTexture(float terrain_size,
                      const std::vector<float>& height_data) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, terrain_size,
               terrain_size, 0, GL_RED, GL_FLOAT,
               height_data.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  Texture32F terrain_texture(
    texture_id, terrain_size, terrain_size,
    GL_RED, GL_R32F, GL_FLOAT
  );
  return terrain_texture;
}

void TerrainRenderer::LoadHmap(JPH::BodyInterface* body_interface, std::string_view hmap_path) {
  const int terrain_size = 1024;
  std::vector<float> height_data(terrain_size * terrain_size, 0.0f);
  int width, height, channels;
  // The '1' at the end forces stb_image to return only the red/greyscale channel
  float* data = stbi_loadf(hmap_path.data(), &width, &height, &channels, 1);

  if (data) {
    std::vector<float> height_data(data, data + (width * height));
    hmap_ = CreateTexture(width, height_data);
    stbi_image_free(data);
  }
  InitializeBody(body_interface, terrain_size, height_data);
}

void TerrainRenderer::LoadDefaultSurface(JPH::BodyInterface* body_interface) {
  const int terrain_size = 1024;
  std::vector<float> height_data(terrain_size * terrain_size, 0.0f);
  std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> dist(0.0f,
                                             1.0f);  // Adjust max height here

  for (float& h : height_data) {
    h = 0.0f * 0.2f * dist(rng);
  }
  hmap_ = CreateTexture(terrain_size, height_data);
  InitializeBody(body_interface, terrain_size, height_data);
}

void TerrainRenderer::InitializeBody(JPH::BodyInterface* body_interface,
    int terrain_size, const std::vector<float>& height_data) {
  float factor = 1.0f / 16.0f;
  JPH::Vec3 offset = JPH::Vec3(-512.0f, 0.0f, -512.0f) * factor;
  JPH::Vec3 scale = JPH::Vec3(1.0f * factor, 1.0f, 1.0f * factor);
  JPH::HeightFieldShapeSettings shape_settings(height_data.data(), offset,
                                               scale, terrain_size);
  shape_settings.mActiveEdgeCosThresholdAngle = 0.996195f;  // ~5 degrees
  JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
  if (shape_result.HasError()) {
    throw std::runtime_error("unable to create hmap floor");
  }
  JPH::ShapeRefC local_shape = shape_result.Get();
  JPH::Vec3 translation(0.0f, 0.0f, 0.0f);
  JPH::Quat rotation = JPH::Quat::sIdentity();
  JPH::EMotionType motion_type = JPH::EMotionType::Static;
  JPH::ObjectLayer object_layer =
      Layers::NON_MOVING /* Your non-moving layer, e.g., Layers::NON_MOVING */;
  JPH::BodyCreationSettings obj_settings(local_shape, translation, rotation,
                                         motion_type, object_layer);
  JPH::EActivation activation_state = JPH::EActivation::DontActivate;
  body_id_ = body_interface->CreateAndAddBody(obj_settings, activation_state);
  // node->shape = local_shape;
  // static_objects_.emplace_back(obj);
}

TerrainRenderData TerrainRenderer::GetRenderData() const noexcept {
  int material_id = 0;
  return {mesh_.GetVao(), GeoClipmaps::gPatchNum, hmap_.GetId(), material_id};
}
