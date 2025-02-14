#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_UIDEBUGGER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_UIDEBUGGER_H_

#include "../common/Paths.h"
#include "../common/Vbos.h"
#include "../common/Details.h"
#include "../io/Window.h"

#include <string_view>
#include <vector>

#include <glad/glad.h>

namespace debug {

void UiScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void UiMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void UiKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

// UiDebugger allows to translate & scale sprite positions.
// If you want to change sprite OR modify transformation manually
// (from the outside), simply rerun the app.
// All manual modification will be discard if the app has been run.
// We haven't add select highlighting and multiple select, but for now
// there's not much usage of it, so skip
class UiDebugger {
 public:
  struct Transform {
    float x_translate;
    float y_translate;
    float scale;
  };

  UiDebugger(const Paths& paths, GLuint vbo_id_coords,
             GLuint vbo_id_transform, const glm::vec2& cursor_pos);

  ~UiDebugger();

  void Update();
  void UpdateForce();

  static void BindCallbacks();

  void TranslateToCursorPos();

  void SetXTranslate(float value);

  void SetYTranslate(float value);

  void SetScale(float value);

  void Press(unsigned int id);

  void Release();

  void ApplyAndReset();

// TODO: --- maybe later ---
  // generate config file for vbo_id_ based on config_path_transform_
//  void ApplyTransforms(std::string_view dest);
  // generate config file for vbo_id_transform_ based on config_path_
//  void GetTransforms(std::string_view dest);

  const float gTranslateStepV = 1.0f / details::kWindowHeight;
  const float gTranslateStepH = 1.0f / details::kWindowWidth;
  const float gScaleStep = 0.01f;

 private:
  void UpdateBuffer(Transform pos);

  // for translate vbo
  int GetVboOffset() const {
    return cur_ui_vbo_id_ * 4;
  }

  void UpdatePosition();

  void Init();

  static std::vector<float> ParseConfigFile(std::string_view path);

  static void SerializeConfigFile(
      std::string_view path,
      std::array<Transform, vbos::gUiVboTransformSize / 3> transforms);

  const Paths& paths_;

  int cur_ui_vbo_id_{-1};

  // position & texture coords
  GLuint vbo_id_;

  // transform (2D scale + translate)
  GLuint vbo_id_transform_;

  const glm::vec2& cursor_pos_;

  Transform delta_transform_{0.0f};

  bool mouse_pressed_{false};
};

extern std::array<UiDebugger::Transform, vbos::gUiVboTransformSize / 3>
    gUiTransforms;

} // namespace debug

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_UIDEBUGGER_H_
