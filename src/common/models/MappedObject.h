#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MAPPEDOBJECT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MAPPEDOBJECT_H_
/*
#include <vector>

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "../Texture.h"
#include "../OpenGlUtility.h"

struct ModelData;

class MappedObject {
 public:
  MappedObject() = default;

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(const ModelData* model) {
    model_data_ = model;
  }

  /// 1.0 for placement in every cell
  void SetPlacement(
      UiSharedResources& ui_shared_resources,
      const Texture* map_placement, float density = 0.1f) {
    density_ = std::clamp(density, 0.01f, 1.0f);
    map_placement_ = map_placement;
    UpdatePlacement(ui_shared_resources);
  }

 private:
  void UpdatePlacement(UiSharedResources& ui_shared_resources) {
    ui_shared_resources.shader_placement_.Bind();
    glUniform1f(density_);
    utility::BindImageTexture(0, map_placement_->GetId(), GL_READ_ONLY);
    glDispatchCompute();
    glMemoryBarrier();
    utility::UnBindImageTexture(0, map_placement_->GetId(), GL_READ_ONLY);
  }

  const ModelData* model_data_ = nullptr;
  const Texture* map_placement_ = nullptr;

  /// GLuint means x*1024+y with underlying terrain Y coord
  /// rotation * scale deduced randomly from GLuint value
  std::vector<GLuint> positions_;
  float density_ = 0.1f;
};*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MAPPEDOBJECT_H_
