#include "UiDebugger.h"

#include <cstring>

#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Ui.h"

namespace debug {

std::array<UiDebugger::Transform, vbos::gUiVboTransformSize / 3>
    gUiTransforms{};

void UiScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto& ui_debugger = reinterpret_cast<GlobalGlfwCallbackData*>(
                          glfwGetWindowUserPointer(window))->ui_debugger_;
  ui_debugger.SetScale(static_cast<float>(yoffset) * ui_debugger.gScaleStep);
}

void UiMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto& ui_debugger = global_data->ui_debugger_;
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      auto id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      ui_debugger.Press(id);
    } else {
      ui_debugger.Release();
    }
  }
}

void UiKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto& ui_debugger = reinterpret_cast<GlobalGlfwCallbackData*>(
                          glfwGetWindowUserPointer(window))->ui_debugger_;
  if (action == GLFW_PRESS) {
    // left control key is handled from Interface class (you should hold it)
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_W) {
      ui_debugger.SetYTranslate(ui_debugger.gTranslateStepV);
    } else if (key == GLFW_KEY_A) {
      ui_debugger.SetXTranslate(-ui_debugger.gTranslateStepH);
    } else if (key == GLFW_KEY_S) {
      ui_debugger.SetYTranslate(-ui_debugger.gTranslateStepV);
    } else if (key == GLFW_KEY_D) {
      ui_debugger.SetXTranslate(ui_debugger.gTranslateStepH);
    } else if (key == GLFW_KEY_E) {
      ui_debugger.SetScale(ui_debugger.gScaleStep);
    } else if (key == GLFW_KEY_Q) {
      ui_debugger.SetScale(-ui_debugger.gScaleStep);
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
      ui_debugger.SetYTranslate(0.0f);
    } else if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
      ui_debugger.SetXTranslate(0.0f);
    } else if (key == GLFW_KEY_E || key == GLFW_KEY_Q) {
      ui_debugger.SetScale(0.0f);
    }
  }
}

UiDebugger::UiDebugger(
    const Paths& paths, GLuint vbo_id_coords,
    GLuint vbo_id_transform, const glm::vec2& cursor_pos)
    : paths_(paths),
      vbo_id_(vbo_id_coords),
      vbo_id_transform_(vbo_id_transform),
      cursor_pos_(cursor_pos) {
  Init();
}

void UiDebugger::Init() {
  auto data = ParseConfigFile(paths_.config_vbo_sprites);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0,
      static_cast<GLsizeiptr>(data.size() * sizeof(float)),
      data.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  data = ParseConfigFile(paths_.config_vbo_transform);
  std::memcpy(gUiTransforms.data(), data.data(), data.size() * sizeof(float));
  UpdateForce();
}

UiDebugger::~UiDebugger() {
  SerializeConfigFile("test.txt", gUiTransforms);
}

void UiDebugger::BindCallbacks() {
  glfwSetScrollCallback(gWindow, UiScrollCallback);
  glfwSetMouseButtonCallback(gWindow, UiMouseButtonCallback);
  glfwSetKeyCallback(gWindow, UiKeyCallback);
}

void UiDebugger::Update() {
  if (cur_ui_vbo_id_ == -1) {
    return;
  }
  if (mouse_pressed_) {
    TranslateToCursorPos();
  }
  UpdatePosition();
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_transform_);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0,
      static_cast<GLsizeiptr>(vbos::gUiVboTransformSize * sizeof(float)),
      gUiTransforms.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UiDebugger::UpdatePosition() {
  if (delta_transform_.x_translate == 0.0f &&
      delta_transform_.y_translate == 0.0f && delta_transform_.scale == 0.0f) {
    return;
  }
  // duplicate 4 times for each vertex
  Transform pos = gUiTransforms[GetVboOffset()];
  pos.x_translate += delta_transform_.x_translate;
  pos.y_translate += delta_transform_.y_translate;
  pos.scale += delta_transform_.scale;
  UpdateBuffer(pos);
}

void UiDebugger::UpdateBuffer(Transform pos) {
  for (int k = 0; k < gUiComponents[cur_ui_vbo_id_].children_num + 1; ++k) {
    // if it's parent - offset is 0 by default
    for (int i = 0; i < 4; ++i) {
      gUiTransforms[GetVboOffset() + i + k * 4] = Transform(
          pos.x_translate + gUiComponents[cur_ui_vbo_id_ + k].parent_offset_x,
          pos.y_translate + gUiComponents[cur_ui_vbo_id_ + k].parent_offset_y,
          pos.scale * gUiComponents[cur_ui_vbo_id_ + k].parent_scale
          );
    }
  }
}

void UiDebugger::UpdateForce() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_transform_);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0,
      static_cast<GLsizeiptr>(vbos::gUiVboTransformSize * sizeof(float)),
      gUiTransforms.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::vector<float> UiDebugger::ParseConfigFile(
    std::string_view path) {
  std::ifstream config_file(path.data());
  if (!config_file) {
    throw std::runtime_error("Unable to open Ui VBO data");
  }
  std::vector<float> data;
  data.reserve(100 * 16); // assume 100 ui components total
  std::string line_buffer, entry_buffer;
  std::istringstream line_stream;
  // no trim for tabs/whitespaces, but we don't need it
  while (std::getline(config_file, line_buffer)) {
    line_stream.clear();
    line_stream.str(line_buffer);
    /// it's either:
    /// 4 floats separated by commas: -0.15,0.2,0.0,1.0,
    /// or
    /// // comment starting with double slash
    /// or
    /// empty line
    if (line_buffer[0] == '/' || line_buffer.empty()) {
      continue;
    }
    try {
      while (std::getline(line_stream, entry_buffer, ',')) {
        data.push_back(std::stof(entry_buffer));
      }
    } catch (...) {
      std::cerr << "unable to parse vbo config file " << path << std::endl;
    }
  }
  return data;
}

void UiDebugger::SerializeConfigFile(
    std::string_view path,
    std::array<Transform, vbos::gUiVboTransformSize / 3> transforms) {
  std::ofstream file(path.data());
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file: " << path << std::endl;
    return;
  }
  std::ostringstream oss_file, oss_line;
  oss_file << "// should be applied to all transforms\n";
  for (int i = 0; i < vbos::gUiVboTransformSize / 3; i += 4) {
    oss_line << std::to_string(transforms[i].x_translate) << ','
             << std::to_string(transforms[i].y_translate) << ','
             << std::to_string(transforms[i].scale) << ",\n"; // x4
    auto per_vertex_line = oss_line.str();
    for (int j = 0; j < 4; ++j) {
      oss_file << per_vertex_line;
    }
    oss_file << '\n';
    oss_line.str("");
    oss_line.clear();
  }
  file << oss_file.str();
}

void UiDebugger::TranslateToCursorPos() {
  float prev_scale = gUiTransforms[GetVboOffset()].scale;
  Transform pos = {cursor_pos_.x, cursor_pos_.y, prev_scale};
  UpdateBuffer(pos);
}

void UiDebugger::SetXTranslate(float value) {
  if (cur_ui_vbo_id_ != -1) {
    delta_transform_.x_translate = value;
  }
}

void UiDebugger::SetYTranslate(float value) {
  if (cur_ui_vbo_id_ != -1) {
    delta_transform_.y_translate = value;
  }
}

void UiDebugger::SetScale(float value) {
  if (cur_ui_vbo_id_ != -1) {
    delta_transform_.scale = value;
  }
}

void UiDebugger::Press(unsigned int id) {
  mouse_pressed_ = true;
  if (id >= static_cast<int>(vbos::VboIdMain::kMenuTerrain) &&
      id < static_cast<int>(vbos::VboIdMain::kTotal)) {
    cur_ui_vbo_id_ = id - static_cast<int>(vbos::VboIdMain::kMenuTerrain);
  } else {
    ApplyAndReset();
  }
}

void UiDebugger::ApplyAndReset() {
  if (cur_ui_vbo_id_ == -1) {
    return;
  }
  auto transform = gUiTransforms[GetVboOffset()];
  auto ui = gUiComponents[cur_ui_vbo_id_].ui;
  if (ui) {
    ui->UpdateTransform(transform.x_translate, transform.y_translate,
                       transform.scale);
  }
  cur_ui_vbo_id_ = -1;
  delta_transform_ = Transform{0.0f, 0.0f, 0.0f};
}

void UiDebugger::Release() {
  mouse_pressed_ = false;
}

} // namespace debug
