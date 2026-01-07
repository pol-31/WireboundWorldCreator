#ifndef WIREBOUNDWORLDCREATOR_UIEDITROADS_H
#define WIREBOUNDWORLDCREATOR_UIEDITROADS_H


#include <array>
#include <vector>

#include "../common/GraphBakeConfig.h"
#include "../common/MapPoint.h"
#include "UiEdit.h"
#include "UiEditShared.h"

struct RoadTraits {
  std::vector<MapPoint> map_points;
  std::vector<glm::uvec2> map_joints;
  GraphBakeConfig config;

  // C++ can’t do pointer-to-nested-member directly?
  float curve_amplitude = 3.0f;
  float height_drift = 0.01f;
  float height_raise = 0.0f;
  float side_sagging = 0.01f;
  float radius_flat = 1.0f;
  float radius = 4.0f;

  static constexpr int GetTraitsSize() noexcept {
    return 6;
  }

  GraphBakeConfig GetGraphConfig() const noexcept;
};

class UiEditRoads : public IUiEdit {
public:
  UiEditRoads(UiSharedResources& ui_shared_resources,
              UiEditSlots& ui_edit_slots,
              UiEditConfigSlTxt& value_config);

  UiEditRoads(UiEditRoads&& other) noexcept;

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

  [[nodiscard]] RoadTraits& GetInstanceData() noexcept;

private:
  std::vector<RoadTraits> instances_;
  std::array<UiEditConfigSlTxt::Trait, RoadTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITROADS_H
