#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void BiomesModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void BiomesModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void BiomesModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class BiomesMode final : public IEditMode {
 public:
  explicit BiomesMode(SharedResources& shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

 protected:
  friend void BiomesModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void BiomesModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void BiomesModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_
