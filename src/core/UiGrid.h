#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_

#include <glad/glad.h>

#include "../render/Shader.h"
#include "../ui/UiRenderData.h"

class UiGrid {
 public:
  UiGrid(UiRenderData& render_data);

  ~UiGrid() { DeInit(); }

  void RenderGrid();

  // scale for UiLayerWireframe axis render
  void RenderAxis(float scale);

  void RenderBoundary();

 private:
  void Init();

  void DeInit();

  UiRenderData& render_data_;
  Shader shader_grid_;
  Shader shader_axis_;
  Shader shader_world_boundary_;
  GLuint vao_;
  GLuint vbo_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIGRID_H_
