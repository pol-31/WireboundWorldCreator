#ifndef WIREBOUNDWORLDCREATOR_UIEDITRIVER_H
#define WIREBOUNDWORLDCREATOR_UIEDITRIVER_H

#include <array>
#include <vector>

#include "UiEdit.h"
#include "UiEditShared.h"
#include "traits/RiverTraits.h"

class UiEditRiver : public IUiEdit {
 public:
  UiEditRiver(UiSharedResources& ui_shared_resources,
              UiEditSlots& ui_edit_slots, UiEditConfigSlTxt& value_config);

  UiEditRiver(UiEditRiver&& other) noexcept;

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

  std::vector<RiverTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] RiverTraits& GetInstanceData() noexcept;

 private:
  std::array<UiEditConfigSlTxt::Trait, RiverTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITRIVER_H
