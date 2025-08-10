#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include "Ui.h"
#include "UiComplex.h"
#include "../modes/UiTerrainWindows.h"

#include "../common/ArbitraryGraph.h"
#include "../common/TerrainGrid.h"

struct UiSlotsSliderData {
  float progress_{0.0f};
  bool pressed_{false};
  float centre_{0.0f};
  float length_{0.0f};
  float length_slots_{0.0f};
  static const float kTrackLengthFactor;
  static const float kSlotsLengthFactor;
  static const int kSlotsNum;

  float slot_height_{0.0f};
  int scissors_start_{0};
  int scissors_length_{0};

  /// so we could get id related to slots (0-5)
  int cur_slots_offset_{0};
  glm::vec2 start_slot_translate_{0.0f};

  void UpdateRenderData(float track_length, int graphs_num);

  int GetSlotId(glm::vec2 mouse_pos);

  void Set(glm::vec2 mouse_pos, UiDynamicSprite& handler_sprite,
           float track_length, int graphs_num);

  void Set(float progress, UiDynamicSprite& handler_sprite,
           float track_length, int graphs_num);

  void FocusOnSelected(int slot_id, int graphs_num,
                       UiDynamicSprite& handler_sprite,
                       const UiDynamicSprite& back_sprite);
};

class IUiSlots : public UiBase {
 public:
  IUiSlots(size_t vbo_texture_id, UiSharedResources& ui_shared_resources);

  /// other ctors default

  /// UiBase methods
  void Press() override;

  void Release() override;

  void UpdateTransform() override;

  /// interface virtual methods
  virtual void Render(glm::vec2 mouse_pos) = 0;
  virtual void RenderPicking() = 0;
  virtual bool Press(int id) = 0;
  virtual void UpdateTransform(
      float x_translate, float y_translate, float scale) = 0;
  virtual bool Scroll(GLuint id, float yoffset) = 0;

  /// as a decorator for graph_
  /*bool Press(int id) {
    if (!InRange()) {
      return false;
    }

  }*/
  void CreateGraph();
  virtual void SelectGraph(GLuint id);
  void RemoveGraph(GLuint id);

  void RenderGraph();

  [[nodiscard]] int GetSize() const noexcept;

  FixedSizeQueue<char, 64>* GetNamePtr(int instance_id) {
    return graph_->GetNamePtr(instance_id);
  }

  virtual void PressGraph(GLuint id) = 0;

 protected:
  /// slider
  UiSlotsSliderData sl_data_;

  /// event handler (hover / press / release)
  IUiEventHandler* ui_event_handler_ = nullptr;

  /// Render custom, but states show/hide pop-up edit window related to *this
  //  IUiEdit* ui_edit_ = nullptr;

  /// graph data + edit window
  IGraph* graph_ = nullptr;// -- no interface, sry;(

  // for shader bindings & mouse pos
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
      UiDynamicSprite&& handler,
      UiDynamicSprite&& slider,
      UiDynamicSprite&& back,
      UiDynamicSprite&& create,
      data::VboIdMain flip_point_edge_back_vbo_texture,
      data::TextId flip_point_edge_back_text_id,
      UiDynamicSprite&& flip_point_edge_sprite,
      UiDynamicSprite&& slot_name,
      UiDynamicSprite&& slot_config,
      UiToggle&& toggle_slot_visible,
      UiDynamicSprite&& slot_back,
      UiDynamicSprite&& slot_color,
      UiDynamicSprite&& slot_remove,
      UiDynamicSprite&& slot_selected,
      const UiSliderV& slider_size,
      const UiSliderV& slider_falloff);

  UiSlotsTerrain(UiSlotsTerrain&& other) noexcept;
  UiSlotsTerrain(const UiSlotsTerrain& other) = delete;

  UiSlotsTerrain& operator=(UiSlotsTerrain&& other) = delete;
  UiSlotsTerrain& operator=(const UiSlotsTerrain& other) = delete;

  void Render(glm::vec2 mouse_pos) override;
  void RenderPicking() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Press(int id) override;
  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void SelectGraph(GLuint id) override;

  void NextClickMode();

  void PressGraph(GLuint id) override;

 private:
  std::vector<TerrainInstanceData> instances_;
  int instances_size_;

  UiDynamicSprite handler_;
  UiDynamicSprite slider_;
  UiDynamicSprite back_;
  UiDynamicSprite create_;
  UiDynamicSprite flip_point_edge_back_;

  UiDynamicSprite flip_point_edge_sprite_;
  UiSpriteTransformation flip_point_edge_;

  UiDynamicSprite slot_name_;
  UiDynamicSprite slot_config_;
  UiToggle toggle_slot_visible_;

  UiDynamicSprite slot_back_;
  UiDynamicSprite slot_color_; //todo; *color in shader
  UiDynamicSprite slot_remove_;
  UiDynamicSprite slot_selected_;

  /// store here, pointers to base class, see explanation at base class

  UiEditTerrain ui_edit_; // IUiEdit* for base

  TerrainGrid graph_; // IGraph* for base

  EditState edit_state_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainSlotsFlipPointEdgeFace) -
      static_cast<int>(data::VboIdMain::kTerrainSlotsName) + 1
      > ui_event_handler_; // IUiEventHandler for base

  UiSharedResources& ui_shared_resources_;
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
      data::TextId flip_select_edit_back_text_id,
      data::VboIdMain flip_point_edge_back_vbo_texture,
      data::TextId flip_point_edge_back_text_id,
      UiDynamicSprite&& flip_select_edit_sprite,
      UiDynamicSprite&& flip_point_edge_sprite,
      UiDynamicSprite&& slot_name,
      UiDynamicSprite&& slot_config,
      UiToggle&& toggle_slot_visible,
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
  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void NextSelectMode();

  void NextClickMode();


  void PressGraph(GLuint id) override;

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
  UiToggle toggle_slot_visible_;

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
