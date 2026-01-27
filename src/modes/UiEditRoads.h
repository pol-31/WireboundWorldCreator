#ifndef WIREBOUNDWORLDCREATOR_UIEDITROADS_H
#define WIREBOUNDWORLDCREATOR_UIEDITROADS_H

#include <array>
#include <vector>

#include "UiEdit.h"
#include "UiEditShared.h"
#include "traits/RoadTraits.h"

class UiEditRoads : public IUiEdit {
 public:
  UiEditRoads(UiSharedResources& ui_shared_resources,
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

  std::vector<RoadTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] RoadTraits& GetInstanceData() noexcept;

 private:
  std::array<UiEditConfigSlTxt::Trait, RoadTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITROADS_H
