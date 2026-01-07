#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_

#include <array>
#include <vector>

#include "../common/models/ModelManager.h"
#include "../core/TileRenderer.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"

struct ObjectTraits {
  std::vector<MapPoint> map_points;
  float hp = 100.0f;
  float speed = 1.0f;
  float attack = 1.0f;
  float attack_speed = 1.0f;

  static constexpr int GetTraitsSize() noexcept {
    return 4;
  }
};

class UiEditObjects : public IUiEdit {
 public:
  UiEditObjects(UiSharedResources& ui_shared_resources,
                UiEditSlots& ui_edit_slots,
               UiEditConfigSlTxt& value_config,
                ModelManager& mdl_manager);

  UiEditObjects(UiEditObjects&& other) noexcept;

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  void RemoveInstance(GLuint id) override;

  void Reset() override;

  void Generate() override;

  void RandomGenerate() override;

  bool Press(int id, float height) override;

  void Release() override;

  void Render(float height) override;

  void RenderPicking(float height) override;

  [[nodiscard]] ObjectTraits& GetInstanceData() noexcept;

 private:
  std::vector<ObjectTraits> instances_;
  std::array<UiEditConfigSlTxt::Trait, ObjectTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  ModelManager& mdl_manager_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
