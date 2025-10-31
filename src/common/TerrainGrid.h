#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_

#include <array>
#include <set>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "IGraph.h"

#include "../modes/TerrainNoiseData.h"
#include "../modes/UiTerrainWIndows.h"
#include "../modes/TerrainInstanceData.h"

/// similar to ArbitraryGraph, but for terrain ui-slots

// no Target {vertex, edge, face} state
// no Press {edit, select} state
class TerrainGrid final : public IGraph {
 public:
  TerrainGrid(UiSharedResources& ui_shared_resources,
              UiEditTerrain& ui_edit_terrain,
              std::vector<TerrainInstanceData>& instances,
              const UiSliderV3& slider_size,
              const UiSliderV3& slider_falloff);

  TerrainGrid(TerrainGrid&& other);

  void CreateGraph() override;

  void SelectGraph(int slot_id) override;

  bool RemoveGraph(int slot_id) override;

  void Render(glm::vec2 mouse_pos) override;

  void RenderPicking();

  GLuint ProjectCursorOnGrid(glm::vec2 mouse_pos);

  void Press(glm::vec2 mouse_pos,
             bool shift_pressed, bool ctrl_pressed);

  void Release() override;

  void Select(glm::vec2 mouse_pos);

  [[nodiscard]] int GetSize() const noexcept override;

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept override {
    return selected_slot_id_;
  }

  BaseInstanceData* GetBaseInstanceData(int id) override {
    return &instances_[id];
  };

  const std::string* GetNameRef(int instance_id) const override {
    return &instances_[instance_id].name;
  }

  std::string* GetNameRef(int instance_id) override {
    return &instances_[instance_id].name;
  }

  TerrainInstanceData* GetInstanceData();

  void SelectVertices(const std::vector<GLuint>& points);

  void SetSelectionMask(const Texture* mask);

  void MoveSelected(float value);

  static constexpr size_t gMaxLayers = 10;
  static constexpr int gRadius = 10;

 private:
  void ClearSelection();

  void UpdateMousePotentialSelection(glm::vec2 mouse_pos);

  bool start_is_end_ = true;

  /// according to fbo picking id's (storing differs)
  /// (no copy, single instance -> std::set)
  std::set<GLuint> selected_vertices_;

  /// SELECT SECTION
  bool pressed_ = false;
  glm::vec2 mouse_check_point_ = glm::vec2{0.0f};

  /// can't use std::array due to stack limitations (1Mb)
  std::vector<TerrainInstanceData>& instances_;

  int selected_slot_id_ = -1;
  UiSharedResources& ui_shared_resources_;
  UiEditTerrain& ui_edit_terrain_;

  Texture tex_selection_;
  Texture tex_potential_selection_;

  const UiSliderV3& slider_size_;
  const UiSliderV3& slider_falloff_;

  Shader vertices_transform_shader_;
  Shader selection_blur_shader_;
  const Texture* selection_mask_ = nullptr;
  Texture selection_mask_blured_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
