#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_

#include <array>
#include <vector>

#include "../common/MapPoint.h"
#include "../core/TileRenderer.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"
#include "traits/BiomeTraits.h"

class UiEditBiomes : public IUiEdit {
 public:
  UiEditBiomes(UiSharedResources& ui_shared_resources,
               UiEditSlots& ui_edit_slots, UiEditConfigSlTxt& value_config);

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

  std::vector<BiomeTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] BiomeTraits& GetInstanceData() noexcept;

 private:
  std::array<UiEditConfigSlTxt::Trait, BiomeTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
