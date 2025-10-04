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

  void Render();

 private:
  void Init();

  void DeInit();

  UiSharedResources& ui_shared_resources_;
  Shader shader_;
  GLuint vao_;
  GLuint vbo_;
};

class UiAxis {
 public:
  UiAxis(UiSharedResources& ui_shared_resources);

  ~UiAxis() {
    DeInit();
  }

  void Render(float width_x, float width_y, float width_z);

 private:
  void Init();

  void DeInit();

  UiSharedResources& ui_shared_resources_;
  Shader shader_;
  GLuint vao_;
  GLuint vbo_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_
