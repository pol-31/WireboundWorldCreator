#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_

#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "../common/Shader.h"

class UiGrid {
 public:
  UiGrid(UiSharedResources& ui_shared_resources);

  ~UiGrid() {
    DeInit();
  }

  void RenderGrid();

  // scale for UiLayerWireframe axis render
  void RenderAxis(float scale);

 private:
  void Init();

  void DeInit();

  UiSharedResources& ui_shared_resources_;
  Shader shader_grid_;
  Shader shader_axis_;
  GLuint vao_;
  GLuint vbo_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_
