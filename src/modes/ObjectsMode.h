#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_OBJECTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_OBJECTMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void ObjectsModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void ObjectsModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void ObjectsModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class ObjectsMode final : public IEditMode {
 public:
  explicit ObjectsMode(SharedResources& shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

 protected:
  friend void ObjectsModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void ObjectsModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void ObjectsModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_OBJECTMODE_H_
