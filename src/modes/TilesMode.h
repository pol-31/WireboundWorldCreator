#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void TilesModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void TilesModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void TilesModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class TilesMode final : public IEditMode {
 public:
  explicit TilesMode(SharedResources& shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

 protected:
  friend void TilesModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void TilesModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void TilesModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_
