#include "UiWaterMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../common/ShadersBinding.h"
#include "../common/ArbitraryGraph.h"

void UiWaterMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    water->ocean_layer_config_1_.Scroll(pressed_id, yoffset);
    water->ocean_layer_config_2_.Scroll(pressed_id, yoffset);
    water->ocean_layer_config_3_.Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->DownScale();
  } else {
    global_data->tile_renderer->UpScale();
  }
}

void UiWaterMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  //TODO: refactor to "if(!cond) return"
  //TODO: change order (based on usage frequency)
  global_data->camera->ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu->Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id < details::kIdOffsetUi && water->do_add_points_) {
          //TODO; make it more explicit
          /// because water offset id goes after terrain ids
          if (pressed_id < details::kIdOffsetWater) {
            water->AddNewPoint(pressed_id);
          }
        } else if (pressed_id == water->btn_bake_lake_.GetId()) {
          water->BakeLake();
        } else if (pressed_id == water->btn_bake_river_.GetId()) {
          water->BakeRiver();
        } else if (pressed_id == water->btn_bake_waterfall_.GetId()) {
          water->BakeWaterfall();
        } else if (pressed_id == water->btn_create_.GetId()) {
          water->Create(pressed_id);
        } else if (pressed_id == water->btn_remove_.GetId()) {
          water->Remove();
        } else if (pressed_id == water->btn_update_.GetId()) {
          water->UpdateOcean(); // TODO: update ocean (only ocean by now
        }
        // have no effect if pressed_id doesn't match
        water->ocean_layer_config_1_.Press(pressed_id);
        water->ocean_layer_config_2_.Press(pressed_id);
        water->ocean_layer_config_3_.Press(pressed_id);
      }
    }
  } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
    //Release
    water->ocean_layer_config_1_.Release();
    water->ocean_layer_config_2_.Release();
    water->ocean_layer_config_3_.Release();
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)
void UiWaterMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  /*  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
      } else if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SpeedUp();
      } else if (key == GLFW_KEY_BACKSPACE) {
        global_data->terrain_.ClearPoints();
      } else if (key == GLFW_KEY_ENTER) {
        global_data->terrain_.Bake();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = true;
      }
    } else if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SlowDown();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = false;
      }
    }*/
}

UiWaterMode::UiWaterMode(
    UiSharedResources& ui_shared_resources,
    const Paths& paths)
    : IUiMode(ui_shared_resources,
              UiStaticSprite{data::VboIdMain::kWaterWaterMode,
                             data::TextId::kNone}),
      btn_bake_lake_(data::VboIdMain::kWaterLake, data::TextId::kNone,
                     [this]() {
                       this->BakeLake();
                     }),
      btn_bake_river_(data::VboIdMain::kWaterRiver, data::TextId::kNone,
                      [this]() {
                        this->BakeRiver();
                      }),
      btn_bake_waterfall_(data::VboIdMain::kWaterWaterfall, data::TextId::kNone,
                          [this]() {
                            this->BakeWaterfall();
                          }),
      btn_create_(data::VboIdMain::kWaterAdd, data::TextId::kNone,
                  [this]() {
                    std::cout << "AddNewPoint" << std::endl;
//                    this->AddNewPoint();
                  }),
      btn_remove_(data::VboIdMain::kWaterRemove, data::TextId::kNone,
                  [this]() {
                    this->Remove();
                  }),
      btn_update_(data::VboIdMain::kWaterUpdate, data::TextId::kNone,
                  [this]() {
                    this->UpdateOcean();
                  }),
      ocean_layer_config_1_(
          UiDynamicSprite{data::VboIdMain::kWaterLayer1Window, data::TextId::kNone, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          UiDynamicSprite{data::VboIdMain::kWaterLayer1, data::TextId::kNone, [](){}},
          UiSliderH{{data::VboIdMain::kWater1ScaleFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1ScaleBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1ScaleHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1ScaleIcon, data::TextId::kNone},
                    1000.0f},
          UiSliderH{{data::VboIdMain::kWater1FetchFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1FetchBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1FetchHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1FetchIcon, data::TextId::kNone},
                    100000.0f},
          UiSliderH{{data::VboIdMain::kWater1SpreadBlendFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1SpreadBlendBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1SpreadBlendHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1SpreadBlendIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater1SwellFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1SwellBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1SwellHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1SwellIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater1PeakEnhancementFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1PeakEnhancementBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1PeakEnhancementHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1PeakEnhancementIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater1ShortWavesFadeFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1ShortWavesFadeBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1ShortWavesFadeHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1ShortWavesFadeIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater1LambdaFill, data::TextId::kNone},
                    {data::VboIdMain::kWater1LambdaBack, data::TextId::kNone},
                    {data::VboIdMain::kWater1LambdaHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater1LambdaIcon, data::TextId::kNone},
                    1.0f}),
      ocean_layer_config_2_(
          UiDynamicSprite{data::VboIdMain::kWaterLayer2Window, data::TextId::kNone, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          UiDynamicSprite{data::VboIdMain::kWaterLayer2, data::TextId::kNone, [](){}},
          UiSliderH{{data::VboIdMain::kWater2ScaleFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2ScaleBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2ScaleHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2ScaleIcon, data::TextId::kNone},
                    1000.0f},
          UiSliderH{{data::VboIdMain::kWater2FetchFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2FetchBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2FetchHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2FetchIcon, data::TextId::kNone},
                    100000.0f},
          UiSliderH{{data::VboIdMain::kWater2SpreadBlendFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2SpreadBlendBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2SpreadBlendHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2SpreadBlendIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater2SwellFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2SwellBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2SwellHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2SwellIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater2PeakEnhancementFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2PeakEnhancementBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2PeakEnhancementHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2PeakEnhancementIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater2ShortWavesFadeFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2ShortWavesFadeBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2ShortWavesFadeHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2ShortWavesFadeIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater2LambdaFill, data::TextId::kNone},
                    {data::VboIdMain::kWater2LambdaBack, data::TextId::kNone},
                    {data::VboIdMain::kWater2LambdaHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater2LambdaIcon, data::TextId::kNone},
                    1.0f}),
      ocean_layer_config_3_(
          UiDynamicSprite{data::VboIdMain::kWaterLayer3Window, data::TextId::kNone, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f, 0.2f}, 1.0f, 0.0f},
          UiDynamicSprite{data::VboIdMain::kWaterLayer3, data::TextId::kNone, [](){}},
          UiSliderH{{data::VboIdMain::kWater3ScaleFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3ScaleBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3ScaleHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3ScaleIcon, data::TextId::kNone},
                    1000.0f},
          UiSliderH{{data::VboIdMain::kWater3FetchFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3FetchBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3FetchHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3FetchIcon, data::TextId::kNone},
                    100000.0f},
          UiSliderH{{data::VboIdMain::kWater3SpreadBlendFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3SpreadBlendBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3SpreadBlendHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3SpreadBlendIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater3SwellFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3SwellBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3SwellHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3SwellIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater3PeakEnhancementFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3PeakEnhancementBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3PeakEnhancementHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3PeakEnhancementIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater3ShortWavesFadeFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3ShortWavesFadeBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3ShortWavesFadeHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3ShortWavesFadeIcon, data::TextId::kNone},
                    1.0f},
          UiSliderH{{data::VboIdMain::kWater3LambdaFill, data::TextId::kNone},
                    {data::VboIdMain::kWater3LambdaBack, data::TextId::kNone},
                    {data::VboIdMain::kWater3LambdaHandler, data::TextId::kNone},
                    {data::VboIdMain::kWater3LambdaIcon, data::TextId::kNone},
                    1.0f}) {}

void UiWaterMode::Render() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_mode_.Render();
  btn_bake_lake_.Render();
  btn_bake_river_.Render();
  btn_bake_waterfall_.Render();
  btn_create_.Render();
  btn_remove_.Render();
  btn_update_.Render();

  ocean_layer_config_1_.Render();
  ocean_layer_config_2_.Render();
  ocean_layer_config_3_.Render();
}

bool UiWaterMode::ConfigModified() {
  return !ocean_layer_config_1_.Modified() &&
         !ocean_layer_config_2_.Modified() &&
         !ocean_layer_config_3_.Modified();
}

void UiWaterMode::UpdateOcean() {
  if (ConfigModified()) {
    return;
  }
  OceanTraits traits = {
      ocean_layer_config_1_.GetOceanLayerTraits(),
      ocean_layer_config_2_.GetOceanLayerTraits(),
      ocean_layer_config_3_.GetOceanLayerTraits()
  };
  TileRenderer* tile_renderer =
      ui_shared_resources_.global_glfw_callback_data_.tile_renderer;
  tile_renderer->water.UpdateOcean(traits);
}

void UiWaterMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_mode_.RenderPicking();
  btn_bake_lake_.RenderPicking();
  btn_bake_river_.RenderPicking();
  btn_bake_waterfall_.RenderPicking();
  btn_create_.RenderPicking();
  btn_remove_.RenderPicking();
  btn_update_.RenderPicking();
  ocean_layer_config_1_.RenderPicking();
  ocean_layer_config_2_.RenderPicking();
  ocean_layer_config_3_.RenderPicking();
}

void UiWaterMode::ReBake() {
  //TODO: for each ui_slots instance update heights
}

void UiWaterMode::BakeLake() {
  std::cout << "bake as a lake" << std::endl;
  //TODO: find convex areas, bare edges, bare vertices
}

void UiWaterMode::BakeRiver() {
  std::cout << "bake as a river" << std::endl;
  //TODO:
}

void UiWaterMode::BakeWaterfall() {
  std::cout << "bake as a waterfall" << std::endl;
  //TODO:
}

void UiWaterMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);

  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera->SetInspectCamera();
}

data::TextId UiWaterMode::Hover(std::uint32_t global_id) {
  ocean_layer_config_1_.Hover(global_id);
  ocean_layer_config_2_.Hover(global_id);
  ocean_layer_config_3_.Hover(global_id);
  return data::TextId::kNone;
}
