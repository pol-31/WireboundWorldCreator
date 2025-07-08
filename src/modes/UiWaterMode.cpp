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
        if (false) {//pressed_id < details::kIdOffsetUi && water->do_add_points_) {
          //TODO; make it more explicit
          /// because water offset id goes after terrain ids
//          if (pressed_id < details::kIdOffsetWater) {
//            water->AddNewPoint(pressed_id);
//          }
        } else if (pressed_id == water->btn_bake_lake_.GetId()) {
          water->BakeLake();
        } else if (pressed_id == water->btn_bake_river_.GetId()) {
          water->BakeRiver();
        } else if (pressed_id == water->btn_bake_waterfall_.GetId()) {
          water->BakeWaterfall();
//        } else if (pressed_id == water->btn_create_.GetId()) {
//          water->Create(pressed_id);
//        } else if (pressed_id == water->btn_remove_.GetId()) {
//          water->Remove();
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
                             data::TextId::kNotYet}),
      btn_bake_lake_(data::VboIdMain::kWaterLake, data::TextId::kNotYet,
                     [this]() {
                       this->BakeLake();
                     }),
      btn_bake_river_(data::VboIdMain::kWaterRiver, data::TextId::kNotYet,
                      [this]() {
                        this->BakeRiver();
                      }),
      btn_bake_waterfall_(data::VboIdMain::kWaterWaterfall, data::TextId::kNotYet,
                          [this]() {
                            this->BakeWaterfall();
                          }),
      btn_update_(data::VboIdMain::kWaterUpdate, data::TextId::kNotYet,
                  [this]() {
                    this->UpdateOcean();
                  }),
      ocean_layer_config_1_(
          {data::VboIdMain::kWaterLayer1Window, data::TextId::kNotYet, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f, 0.2f}, 1.0f, 0.0f},
          {{data::VboIdMain::kWaterLayer1PinBack, data::TextId::kNotYet, [](){}},
          {data::VboIdMain::kWaterLayer1PinPoint, data::TextId::kNotYet, [](){}}},
          {data::VboIdMain::kWaterLayer1, data::TextId::kNotYet, [](){}},
          {data::VboIdMain::kWaterLayer1Text, data::TextId::kNotYet, [](){}},
          {{data::VboIdMain::kWaterLayer1Off, data::TextId::kNotYet},
          {data::VboIdMain::kWaterLayer1On1, data::TextId::kNotYet},
          {data::VboIdMain::kWaterLayer1On2, data::TextId::kNotYet},
          {data::VboIdMain::kWaterLayer1On3, data::TextId::kNotYet}},
          {data::VboIdMain::kWater1ScaleLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1ScaleFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1ScaleBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1ScaleHandler, data::TextId::kNotYet},
           1000.0f},
          {data::VboIdMain::kWater1FetchLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1FetchFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1FetchBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1FetchHandler, data::TextId::kNotYet},
           100000.0f},
          {data::VboIdMain::kWater1SpreadBlendLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1SpreadBlendFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1SpreadBlendBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1SpreadBlendHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater1SwellLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1SwellFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1SwellBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1SwellHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater1PeakEnhancementLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1PeakEnhancementFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1PeakEnhancementBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1PeakEnhancementHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater1ShortWavesFadeLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1ShortWavesFadeFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1ShortWavesFadeBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1ShortWavesFadeHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater1LambdaLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater1LambdaFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater1LambdaBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater1LambdaHandler, data::TextId::kNotYet},
           1.0f}),
      ocean_layer_config_2_(
          {data::VboIdMain::kWaterLayer2Window, data::TextId::kNotYet, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f, 0.2f}, 1.0f, 0.0f},
          {{data::VboIdMain::kWaterLayer2PinBack, data::TextId::kNotYet, [](){}},
           {data::VboIdMain::kWaterLayer2PinPoint, data::TextId::kNotYet, [](){}}},
          {data::VboIdMain::kWaterLayer2, data::TextId::kNotYet, [](){}},
          {data::VboIdMain::kWaterLayer2Text, data::TextId::kNotYet, [](){}},
          {{data::VboIdMain::kWaterLayer2Off, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer2On1, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer2On2, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer2On3, data::TextId::kNotYet}},
          {data::VboIdMain::kWater2ScaleLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2ScaleFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2ScaleBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2ScaleHandler, data::TextId::kNotYet},
           1000.0f},
          {data::VboIdMain::kWater2FetchLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2FetchFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2FetchBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2FetchHandler, data::TextId::kNotYet},
           100000.0f},
          {data::VboIdMain::kWater2SpreadBlendLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2SpreadBlendFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2SpreadBlendBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2SpreadBlendHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater2SwellLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2SwellFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2SwellBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2SwellHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater2PeakEnhancementLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2PeakEnhancementFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2PeakEnhancementBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2PeakEnhancementHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater2ShortWavesFadeLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2ShortWavesFadeFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2ShortWavesFadeBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2ShortWavesFadeHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater2LambdaLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater2LambdaFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater2LambdaBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater2LambdaHandler, data::TextId::kNotYet},
           1.0f}),
      ocean_layer_config_3_(
          {data::VboIdMain::kWaterLayer3Window, data::TextId::kNotYet, [](){}},
          1.4f,
          ui_shared_resources,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f, 0.2f}, 1.0f, 0.0f},
          {{data::VboIdMain::kWaterLayer3PinBack, data::TextId::kNotYet, [](){}},
           {data::VboIdMain::kWaterLayer3PinPoint, data::TextId::kNotYet, [](){}}},
          {data::VboIdMain::kWaterLayer3, data::TextId::kNotYet, [](){}},
          {data::VboIdMain::kWaterLayer3Text, data::TextId::kNotYet, [](){}},
          {{data::VboIdMain::kWaterLayer3Off, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer3On1, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer3On2, data::TextId::kNotYet},
           {data::VboIdMain::kWaterLayer3On3, data::TextId::kNotYet}},
          {data::VboIdMain::kWater3ScaleLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3ScaleFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3ScaleBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3ScaleHandler, data::TextId::kNotYet},
           1000.0f},
          {data::VboIdMain::kWater3FetchLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3FetchFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3FetchBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3FetchHandler, data::TextId::kNotYet},
           100000.0f},
          {data::VboIdMain::kWater3SpreadBlendLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3SpreadBlendFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3SpreadBlendBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3SpreadBlendHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater3SwellLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3SwellFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3SwellBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3SwellHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater3PeakEnhancementLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3PeakEnhancementFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3PeakEnhancementBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3PeakEnhancementHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater3ShortWavesFadeLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3ShortWavesFadeFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3ShortWavesFadeBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3ShortWavesFadeHandler, data::TextId::kNotYet},
           1.0f},
          {data::VboIdMain::kWater3LambdaLabel, data::TextId::kNotYet},
          {{data::VboIdMain::kWater3LambdaFill, data::TextId::kNotYet},
           {data::VboIdMain::kWater3LambdaBack, data::TextId::kNotYet},
           {data::VboIdMain::kWater3LambdaHandler, data::TextId::kNotYet},
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
  btn_update_.Render();

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_shader_.Bind();
  ocean_layer_config_1_.Render();
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_shader_.Bind();
  ocean_layer_config_2_.Render();
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_shader_.Bind();
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
  btn_update_.RenderPicking();

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  ocean_layer_config_1_.RenderPicking();
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  ocean_layer_config_2_.RenderPicking();
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
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
