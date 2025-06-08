#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../common/Vbos.h"
#include "../core/UiSlots.h"
#include "../common/Paths.h"
#include "../common/VectorWithSize.h"

class UiWaterMode final : public IUiMode {
 public:
  explicit UiWaterMode(UiSharedResources& ui_shared_resources,
                     const Paths& paths);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void Create(GLuint id);

  /// removes current point set OR removes selected graph object
  void Remove();

  /**
   * Lake: fill everything with height = max_height_ inside the polygon;
   * River: inside the polygon all heights = max_height_, while outside
   *   we bake wrt terrain height;
   * Waterfall: always bake wrt terrain height
   */
  void BakeLake();
  void BakeRiver();
  void BakeWaterfall();

  /// for UiSlots to select certain point data / graph
  void Select(std::size_t idx);

  void BindCallbacks() override;

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  enum class WaterType {
    kLake,
    kRiver,
    kWaterfall,
  };
  void Init();
  void RenderPoints();
  bool FloodFillStablePass();
  bool FloodFillMovingPass();
  void FloodFill();
  void AddNewPoint(std::uint32_t id);
  void ReBake();

  void UpdateOcean();

  bool ConfigModified();

  /// currently only CPU
  /// checks are points indices in ccw order, and
  /// is points polygon convex (only convex allowed)
//  std::vector<Point> GenControlPoints();

  /// these two init height for given points based on WaterType and user params
  /// TODO: (2 user params)

  /// stable means TODO:..
//  void InitStableArea(
//      const std::vector<Point>& control_points);
//  void InitMovingArea(
//      const std::vector<Point>& control_points);

  struct WaterInstance {
    WaterType type;
    std::vector<GLuint> points;
    // describes slope. By default, it directed by gravity (down)
    glm::vec3 gradient{0.0f, -1.0f, 0.0f};
  };

  // idx for points_data_, -1 means no selected
  int cur_points_data_idx_{-1};

  VectorWithSize<WaterInstance> water_data_;

  Shader points_shader_;
  GLuint points_vbo_{0};
  GLuint points_vao_{0};

  /// optimize frequency of point_vbo_ updating
  // for the first time intentionally true to init data;
  bool points_vbo_modified_{true};

  /// stores max water height for current FloodFill(); useful for lake baking
  int max_height_{std::numeric_limits<int>::min()};

  /// -1 as "not selected"
  GLuint cur_selected_point_{static_cast<GLuint>(-1)}; //TODO: editing (not for now)

  UiStaticSprite btn_bake_lake_;
  UiStaticSprite btn_bake_river_;
  UiStaticSprite btn_bake_waterfall_;
  UiStaticSprite btn_create_;
  UiStaticSprite btn_remove_;
  UiStaticSprite btn_update_;
  //UiSlots slots_;

  //TODO: user should set it
  int user_desired_river_raise_{10};

  //TODO: we don't have BakeWaterfall() but rather one more UiSlider, so
  // user can choose start_speed / spread_factor what defines in which
  // cases water should spread not only by direct direction, but to left or right
  // It can be visualized with __speed__ or __angle__
  int user_desired_waterfall_threshold_{};

  /// if we pressed Create() we go there and each pressed terrain id
  /// is considered as a new point for point set
  bool do_add_points_{false}; //TODO rename to creation_mode_

  UiWaterLayerConfig ocean_layer_config_1_;
  UiWaterLayerConfig ocean_layer_config_2_;
  UiWaterLayerConfig ocean_layer_config_3_;

  OceanTraits CollectOceanTraits() {
    return {ocean_layer_config_1_.GetOceanLayerTraits(),
            ocean_layer_config_2_.GetOceanLayerTraits(),
            ocean_layer_config_3_.GetOceanLayerTraits()
    };
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
