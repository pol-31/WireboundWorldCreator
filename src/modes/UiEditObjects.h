#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_

#include <array>
#include <vector>

#include "../common/models/ModelManager.h"
#include "../core/TileRenderer.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "../ui/UiRenderData.h"
#include "traits/ObjectTraits.h"

class UiEditObjects : public IUiEdit {
 public:
  UiEditObjects(UiRenderData& render_data,
                UiEditSlots& ui_edit_slots, UiEditConfigSlTxt& value_config,
                ModelManager& mdl_manager);

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

  std::vector<ObjectTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] ObjectTraits& GetInstanceData() noexcept;

  void SetModels(const std::vector<std::unique_ptr<ModelData>>& models);

 private:
  std::array<UiEditConfigSlTxt::Trait, ObjectTraits::GetTraitsSize()> traits_;
  UiEditConfigSlTxt& value_config_;
  ModelManager& mdl_manager_;
  UiRenderData& render_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
