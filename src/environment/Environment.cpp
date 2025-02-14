#include "Environment.h"

#include <glad/glad.h>

#include "../common/OpenGlUtility.h"
#include "../common/ShadersBinding.h"

Environment::Environment(const Paths& paths)
    : ubo_(),
      time_(ubo_.GetId()),
      wind_(paths, ubo_.GetId()) {}

Environment::Ubo::Ubo() {
  Init();
}

void Environment::Ubo::Init() {
  // time, delta_time, vec2 wind velocity
  utility::InitUbo(id_, sizeof(float) * 4,
                   GL_DYNAMIC_DRAW, shader::kUboEnvironmentBind);
}

Environment::Ubo::~Ubo() {
  DeInit();
}

void Environment::Ubo::DeInit() {
  glDeleteBuffers(1, &id_);
}

void Environment::Update() {
  time_.Update();
  if (static_cast<int>(time_.GetLastTime()) % 4 == 0) {
    wind_.Update(4.8f + time_.GetLastTime(), 20.0f);
  }
}
