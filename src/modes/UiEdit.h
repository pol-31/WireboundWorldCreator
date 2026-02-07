#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDIT_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDIT_H_

#include <random>

#include "../common/BaseInstanceData.h"
#include "../core/UiComplex.h"

class IUiEdit;

class UiEditSlots final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiEditSlots(UiRenderData& render_data, WindowQueue& window_queue,
              TextRenderer& text_renderer);

  void SetUp(IUiEdit* ui_edit);

  void SetInstance(int id);

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  IUiEdit* ui_edit_ = nullptr;

  std::mt19937 random_generator_;

  UiSprite accept_;
  UiTextInput name_;
  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiSprite color_indicator_;
  UiSprite random_generate_;

  UiEventHandler ui_event_handler_;
  UiHierarchy hierarchy_;
  UiRenderData& render_data_;
};

class IUiEdit {
 public:
  IUiEdit(UiEditSlots& ui) : ui_(ui) {}

  virtual ~IUiEdit() = default;

  virtual void HideAll() = 0;
  virtual void CreateInstance() = 0;
  virtual void UpdateConfig() = 0;
  virtual void SetInstanceId(int id) = 0;
  virtual int GetInstancesNum() = 0;

  virtual void RemoveInstance(GLuint id) = 0;
  virtual void Reset() = 0;

  virtual void Generate() = 0;
  virtual void GenerateAll() {}
  virtual void RandomGenerate() = 0;

  virtual bool Press(int id, float height) = 0;
  virtual void Release() = 0;
  virtual void Render(float height) = 0;
  virtual void RenderPicking(float height) = 0;

  virtual BaseInstanceData* GetBaseInstanceData(int id) = 0;

  void SetUp() { ui_.SetUp(this); }

  void Show() { ui_.Show(); }

  int* GetSelectedIdPtr() { return &selected_id_; }

  UiEditSlots& ui_;
  int selected_id_ = -1;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDIT_H_
