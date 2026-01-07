#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_

#include <array>
#include <vector>

#include "../common/MapPoint.h"
#include "../core/TileRenderer.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"

struct BiomeTraits {
  std::vector<MapPoint> map_points;
  float wind_angle = 0.0f;
  float wind_speed = 0.0f;
  float sun_x = -1.0f;
  float sun_y = -1.0f;
  float sun_z = 0.0f;
  float sun_r = 1.0f;
  float sun_g = 1.0f;
  float sun_b = 1.0f;

  static constexpr int GetTraitsSize() noexcept {
    return 8;
  }
};

class UiEditBiomes : public IUiEdit {
 public:
  UiEditBiomes(UiSharedResources& ui_shared_resources,
               UiEditSlots& ui_edit_slots,
               UiEditConfigSlTxt& value_config);

  UiEditBiomes(UiEditBiomes&& other) noexcept;

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

  [[nodiscard]] BiomeTraits& GetInstanceData() noexcept;

 private:
  std::vector<BiomeTraits> instances_;
  std::array<UiEditConfigSlTxt::Trait, BiomeTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
