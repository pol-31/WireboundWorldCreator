#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICLONING_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICLONING_H_

#include <glm/glm.hpp>
#include <iostream>

// TODO: inherit from UiWindowBase, add sprites, make similar to UiSelection
class UiCloning {
 public:
  void Start(glm::vec2 mouse_pos) {
    std::cout << "StartCloning not implemented" << std::endl;
  }

  void Stop(glm::vec2 mouse_pos) {
    std::cout << "StopCloning not implemented" << std::endl;
  }

  void Update(glm::vec2 mouse_pos) {
    std::cout << "UpdateCloning not implemented" << std::endl;
  }

  void ShowConfigWindow() {
    std::cout << "ShowConfigWindow not implemented" << std::endl;
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICLONING_H_
