#ifndef WIREBOUNDWORLDCREATOR_UIEDITOCEAN_H
#define WIREBOUNDWORLDCREATOR_UIEDITOCEAN_H

#include <glad/glad.h>

#include <array>
#include <vector>

#include "../core/UiConfigWindow.h"
#include "../ui/UiRenderData.h"
#include "UiEdit.h"
#include "UiEditShared.h"
#include "UiOceanConfig.h"

class UiEditOcean : public IUiEdit {
 public:
  UiEditOcean(UiRenderData& render_data, TextRenderer& text_renderer,
              UiEditSlots& ui_edit_slots, UiEditConfigSlCfg& value_config,
              UiConfigWindow& ui_ocean_config);

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  void RemoveInstance(GLuint id) override;

  void Reset() override;

  void Generate() override;

  void GenerateAll() override;

  void RandomGenerate() override;

  bool Press(int id, float height) override;

  void Release() override;

  void Render(float height) override;

  void RenderPicking(float height) override;

  std::vector<OceanTraits>& Data();

  BaseInstanceData* GetBaseInstanceData(int id) override { return &Data()[id]; }

  int GetInstancesNum() override { return Data().size(); }

  [[nodiscard]] OceanTraits& GetInstanceData() noexcept;

 private:
  UiEditConfigSlCfg& value_config_;
  UiConfigWindow& ui_ocean_config_;
  std::array<OceanLayerConfig, 3> ocean_layers_;
  UiRenderData& render_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIEDITOCEAN_H
