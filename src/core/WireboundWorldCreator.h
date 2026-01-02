#ifndef WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_

#include "../common/GlfwContext.h"
#include "../common/PickingFramebuffer.h"
#include "../io/Camera.h"
#include "../renderers/UiRenderer.h"
#include "Cubemap.h"
#include "TileRenderer.h"

class WireboundWorldCreator {
 public:
  WireboundWorldCreator();

  ~WireboundWorldCreator() { DeInit(); }

  void RunRenderLoop();

 private:
  void Init();

  void DeInit();

  /// throw an exception in case of uninitialized global_data_ members
  void CheckGlobalData();

  Camera camera_;
  Cubemap cubemap_;

  GlfwContext global_data_;

  PickingFramebuffer picking_fbo_;

  TileRenderer tile_renderer_;

  UiRenderer ui_renderer_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
