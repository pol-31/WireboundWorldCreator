#ifndef WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_

#include "../common/Paths.h"
#include "../io/Camera.h"
#include "Cubemap.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../common/PickingFramebuffer.h"
#include "TileRenderer.h"
#include "../renderers/UiRenderer.h"

class WireboundWorldCreator {
 public:
  explicit WireboundWorldCreator(const Paths& paths);

  ~WireboundWorldCreator();

  void RunRenderLoop();

 private:
  void Init(const Paths& paths);

  void DeInit();

  /// throw an exception in case of uninitialized global_data_ members
  void CheckGlobalData();

  Camera camera_;
  Cubemap cubemap_;

  GlobalGlfwCallbackData global_data_;

  PickingFramebuffer picking_fbo_;

  TileRenderer tile_renderer_;

  UiRenderer ui_renderer_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
