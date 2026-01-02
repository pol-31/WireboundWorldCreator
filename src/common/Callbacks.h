#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_CALLBACKS_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_CALLBACKS_H_

struct GLFWwindow;

namespace callbacks {

void MouseButtonCallback_Mmb_MmbShift(GLFWwindow* window, int button,
                                      int action, int mods);

void KeyCallback_Blocked(GLFWwindow* window, int key, int scancode, int action,
                         int mods);

void CursorPosCallback_Mmb(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_MmbShift(GLFWwindow* window, double xpos, double ypos);

}  // namespace callbacks

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_CALLBACKS_H_
