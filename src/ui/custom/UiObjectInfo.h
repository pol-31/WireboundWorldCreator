#ifndef WIREBOUNDWORLDCREATOR_UIOBJECTINFO_H
#define WIREBOUNDWORLDCREATOR_UIOBJECTINFO_H

#include "../UiElement.h"

class UiObjectInfo final : public UiElement {
  // public:
  //   using Base = UiWindowAppear;
  //   using Base::Show;
  //
  //   UiObjectInfo(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
  //                UiRenderData& render_data,
  //                WindowQueue& window_queue,
  //
  //                UiSprite&& sp_enemy, UiSprite&& sp_friend, UiSprite&&
  //                sp_neutral, UiSprite&& sp_obstacle, UiText&& txt_name,
  //                UiText&& txt_characteristic, UiText&& txt_value);
  //
  //   UiObjectInfo(UiObjectInfo&& other) = delete;
  //   UiObjectInfo(const UiObjectInfo& other) = delete;
  //
  //   UiObjectInfo& operator=(UiObjectInfo&& other) = delete;
  //   UiObjectInfo& operator=(const UiObjectInfo& other) = delete;
  //
  //   bool Render() override;
  //
  //   void RenderPicking() override;
  //
  //   bool Press(int id) override;
  //
  //   void Release() override;
  //
  //   void Show(bool enemy_selected, bool friend_selected, bool
  //   neutral_selected,
  //             bool obstacle_selected, int selected_num);
  //
  //   // if selected_num == 1
  //   void Show(bool enemy_selected, bool friend_selected, bool
  //   neutral_selected,
  //             bool obstacle_selected, const ModelData* mdl_data);
  //
  // private:
  //   struct ObjParam {
  //     std::string_view name;
  //     float value;
  //   };
  //
  //   void RenderParams();
  //
  //   void RenderPickingParams();
  //
  //   UiSprite sp_enemy_;
  //   UiSprite sp_friend_;
  //   UiSprite sp_neutral_;
  //   UiSprite sp_obstacle_;
  //   UiText txt_name_;
  //   UiText txt_characteristic_;
  //   UiText txt_value_;
  //
  //   bool enemy_selected_ = false;
  //   bool friend_selected_ = false;
  //   bool neutral_selected_ = false;
  //   bool obstacle_selected_ = false;
  //
  //   int selected_num_ = 0;
  //   const ModelData* mdl_data_ = nullptr;
  //
  //   std::vector<ObjParam> render_params_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIOBJECTINFO_H
