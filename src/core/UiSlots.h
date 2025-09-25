#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include "Ui.h"
#include "UiComplex.h"
#include "../modes/UiTerrainWindows.h"

#include "../common/ArbitraryGraph.h"
#include "../common/TerrainGrid.h"

class IUiSlots : public UiBase {
 public:
  IUiSlots(size_t vbo_texture_id,
           UiSharedResources& ui_shared_resources,
           UiWindowSlider&& sl_data);

  void Press() override;

  void Release() override;

  virtual void Render(glm::vec2 mouse_pos) = 0;
  virtual void RenderPicking() = 0;
  virtual bool Press(int id) = 0;
  virtual bool Scroll(GLuint id, float yoffset) = 0;

  void CreateGraph();

  virtual void SelectGraph(GLuint id);

  void RemoveGraph(GLuint id);

  void RenderGraph();

  [[nodiscard]] int GetSize() const noexcept;

  std::string* GetNameRef(int instance_id) {
    return graph_->GetNameRef(instance_id);
  }

  virtual void PressGraph(GLuint id) = 0;

 protected:
  UiWindowSlider sl_data_;
  IUiEventHandler* ui_event_handler_ = nullptr;
  IGraph* graph_ = nullptr;
  UiSharedResources& ui_shared_resources_;
};

enum class EditState {
  kVertices,
  kEdges,
  kFaces
};

enum class PressState {
  kSelect,
  kModify
};

class UiSlotsTerrain final : public IUiSlots {
 public:
  UiSlotsTerrain(
      Tile& cur_tile,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      const UiSliderV3& slider_size,
      const UiSliderV3& slider_falloff);

  UiSlotsTerrain(UiSlotsTerrain&& other) noexcept;
  UiSlotsTerrain(const UiSlotsTerrain& other) = delete;

  UiSlotsTerrain& operator=(UiSlotsTerrain&& other) = delete;
  UiSlotsTerrain& operator=(const UiSlotsTerrain& other) = delete;

  void Render(glm::vec2 mouse_pos) override;

  void RenderPicking() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Press(int id) override;

  void SelectGraph(GLuint id) override;

  void TranslateSelected(glm::vec3 value);

  void RotateSelected(glm::vec3 value);

  void ScaleSelected(glm::vec3 value);

  int GetSlotId();

  void UpdateTransform() override;

  void PressGraph(GLuint id) override {};

 private:
  void RenderSlotsSprites();

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  std::vector<TerrainInstanceData> instances_;

//  UiDynamicSprite handler_;
//  UiDynamicSprite slider_;
  UiDynamicSprite back_;
  UiDynamicSprite create_;

  UiText slot_name_;
  UiDynamicSprite slot_config_;
  UiToggle4 toggle_slot_visible_;

  UiDynamicSprite slot_back_;
  UiDynamicSprite slot_color_;
  UiDynamicSprite slot_remove_;
  UiDynamicSprite slot_selected_;

  UiEditTerrain ui_edit_;
  TerrainGrid graph_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainSlotsCreate) -
      static_cast<int>(data::VboIdMain::kTerrainSlotsName) + 1
      > ui_event_handler_; // IUiEventHandler for base

  UiSharedResources& ui_shared_resources_;
  UiHierarchy hierarchy_;
};

class UiSlotsWater final : public IUiSlots {
 private:
  EditState edit_state_;
  PressState press_state_;
};

/// related to roads, fences, biomes
class UiSlotsModels final : public IUiSlots {
 public:
  UiSlotsModels(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& handler,
      UiDynamicSprite&& slider,
      UiDynamicSprite&& back,
      UiDynamicSprite&& create,
      data::VboIdMain flip_select_edit_back_vbo_texture,
      data::VboIdMain flip_point_edge_back_vbo_texture,
      UiDynamicSprite&& flip_select_edit_sprite,
      UiDynamicSprite&& flip_point_edge_sprite,
      UiDynamicSprite&& slot_name,
      UiDynamicSprite&& slot_config,
                UiToggle4&& toggle_slot_visible,
      UiDynamicSprite&& slot_back,
      UiDynamicSprite&& slot_color,
      UiDynamicSprite&& slot_remove,
      UiDynamicSprite&& slot_selected);

  UiSlotsModels(UiSlotsModels&& other) noexcept;
  UiSlotsModels(const UiSlotsModels& other) = delete;

  UiSlotsModels& operator=(UiSlotsModels&& other) = delete;
  UiSlotsModels& operator=(const UiSlotsModels& other) = delete;

  void Render(glm::vec2 mouse_pos) override;
  void RenderPicking() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Press(int id) override;

  void NextSelectMode();

  void NextClickMode();

  void PressGraph(GLuint id) override;

  void UpdateTransform() override;

 private:
  UiDynamicSprite handler_;
  UiDynamicSprite slider_;
  UiDynamicSprite back_;
  UiDynamicSprite create_;
  UiDynamicSprite flip_select_edit_back_;
  UiDynamicSprite flip_point_edge_back_;

  UiDynamicSprite flip_select_edit_sprite_;
  UiDynamicSprite flip_point_edge_sprite_;
  UiSpriteTransformation flip_select_edit_;
  UiSpriteTransformation flip_point_edge_;

  UiDynamicSprite slot_name_;
  UiDynamicSprite slot_config_;
  UiToggle4 toggle_slot_visible_;

  UiDynamicSprite slot_back_;
  UiDynamicSprite slot_color_; //todo; *color in shader
  UiDynamicSprite slot_remove_;
  UiDynamicSprite slot_selected_;

  /// store here, pointers to base class, see explanation at base class

  ArbitraryGraph graph_; // IGraph* for base

  UiEditFences ui_edit_; // IUiEdit* for base

  EditState edit_state_;
  PressState press_state_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kFencesSlotsFlipSelectEdit) -
      static_cast<int>(data::VboIdMain::kFencesSlotsName) + 1
      > ui_event_handler_; // IUiEventHandler for base

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
