#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"
#include "../common/Paths.h"
#include "../common/VectorWithSize.h"

void WaterModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void WaterModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void WaterModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class WaterMode final : public IEditMode {
 public:
  enum class WaterType {
    kLake,
    kRiver,
    kWaterfall,
  };

  explicit WaterMode(SharedResources& shared_resources,
                     const Paths& paths);

  void Render() override;
  void RenderPicking() override;

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
  // protected, but not private because of GLFW callbacks,
  // that are c-style functions (global scope)
  //TODO: we still can make private functions like Init() or
  // fields not used in callbacks OR add protected setters, etc...

  friend void WaterModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void WaterModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void WaterModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  void Init();

  void RenderPoints();

  //TODO: OPTIMIZE!!! (e.g. we can check only with water and only with borders)
  //TODO: gpu / concurrency ?
  /// cpu; returns false if the water has stabilized
  /// (i.e. no changes after the flood fill)

  /// stable means TODO:..
  bool FloodFillStablePass();
  bool FloodFillMovingPass();

  void FloodFill();

  void AddNewPoint(std::uint32_t id);

  void ReBake();

  /// currently only CPU
  /// checks are points indices in ccw order, and
  /// is points polygon convex (only convex allowed)
  std::vector<Point> GenControlPoints();

  /// these two init height for given points based on WaterType and user params
  /// TODO: (2 user params)

  /// stable means TODO:..
  void InitStableArea(
      const std::vector<Point>& control_points);
  void InitMovingArea(
      const std::vector<Point>& control_points);

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

  UiButton btn_bake_lake_;
  UiButton btn_bake_river_;
  UiButton btn_bake_waterfall_;
  UiButton btn_create_;
  UiButton btn_remove_;
  UiSlots slots_;

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
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
