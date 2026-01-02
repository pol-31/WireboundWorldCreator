#ifndef WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../common/Details.h"

class Cursor {
 public:
  Cursor();

  void SetModeDisabled();
  void SetModeEnabled();

 private:
  double last_x_;
  double last_y_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CURSOR_H_
