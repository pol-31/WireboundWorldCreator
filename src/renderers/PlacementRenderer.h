#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_

#include "../core/Tile.h"
#include "../common/Paths.h"

#include "placement/AllPlacement.h"

class PlacementRenderer {
 public:
  PlacementRenderer(Tile& tile, const Paths& paths);

  void Render();

  void RenderDraw() const;

 private:
  void Init();

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};
  Shader shader_;

  Grass grass_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
