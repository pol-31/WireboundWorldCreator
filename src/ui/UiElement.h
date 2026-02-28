#ifndef WIREBOUNDWORLDCREATOR_UI_UIELEMENT_H
#define WIREBOUNDWORLDCREATOR_UI_UIELEMENT_H

#include "../common/Vbos.h"
#include "UiQuad.h"

// for derived
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

// separate Set() and SetInternal()

// UiVerticalStack, UiHorizontalStack - only AutoLayout() override

// TODO: combobox (not appearing list, not list, not smt else)

// class UiTipWindow final : public UiWindowAppear;

// TODO: seems Contain() is better than picking..
//  for picking we need then std::map. Let's keep both for now
//  but anyway we check picking framebuffer and rewrite it as well, let's try

// TODO: let's try rely on A-channel for picking, should be super-cool
//  for now picking pass sprites same as default pass, simply render to 2-tx fbo

class UiElement {
 public:
  enum class Alignment {
    Left,
    Center,
    Right,
  };

  UiElement() = default;

  UiElement(data::UiId id) : id_(id) {}

  virtual ~UiElement() { Clear(); }

  /// --- hierarchy

  void Add(UiElement* inElement);

  void Clear();

  [[nodiscard]] bool Contains(int inX, int inY) const;

  [[nodiscard]] bool ContainsWidened(int inX, int inY, int inBorder) const;

  UiElement* FindByID(int inID);

  /// resets all local transforms set by Translate / Rotate / Scale to in_file;
  /// aligns all components, updated cached absolute positions (todo;?)
  virtual void AutoLayout();

  /// read all ui transforms from file, then AutoLayout read from global data
  static void UpdateDbgTransform();

  /// order matters: FIFO render
  /// linear transformation, so collect pieces this way
  virtual void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                      float rotate, glm::vec2 scale);

  void Update(float delta_time, UiLayer& ui_layer) {
    Update(delta_time, ui_layer, glm::vec2(0.0f), 0.0f, glm::vec2(1.0f));
  }

  /// --- input (id-related, not for hierarchy)
  /// for better effects use Contains() of ContainsWidened, while
  /// these functions designed to use after FindByID() call

  virtual void Press(unsigned int pressed_id) {}

  virtual void Release() {}

  virtual void MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) {}

  virtual void Scroll(unsigned int id, float yoffset) {}

  /// --- getters & setters

  [[nodiscard]] data::UiId GetId() const noexcept { return id_; }

  void Translate(glm::vec2 value) { position_ += value; }

  void Rotate(float value) { rotate_ += value; }

  void Scale(glm::vec2 value) { scale_ *= value; }

  [[nodiscard]] glm::vec2 GetPosition() const noexcept { return position_; }

  void SetPosition(glm::vec2 position) { position_ = position; }

  [[nodiscard]] float GetRotate() const noexcept { return rotate_; }

  void SetRotate(float rotate) { rotate_ = rotate; }

  [[nodiscard]] glm::vec2 GetScale() const noexcept { return scale_; }

  void SetScale(glm::vec2 scale) { scale_ = scale; }

 protected:
  data::UiId id_ = static_cast<data::UiId>(0);

  UiElement* mParent = nullptr;
  std::vector<UiElement*> mChildren;

  /// UiPosDbg.txt + parent_transform_
  glm::vec2 position_ = glm::vec2(0.0f);
  float rotate_ = 0.0f;
  glm::vec2 scale_ = glm::vec2(1.0f);

  /// alignment, AutoLayout() based on children
  Alignment alignment_x_ = Alignment::Center;
  Alignment alignment_y_ = Alignment::Center;
  glm::vec2 width_ = glm::vec2(1.0f);  // for alignment, based on children size
  glm::vec2 padding_ = glm::vec2(0.0f);  // related to width_x_
};

#endif  // WIREBOUNDWORLDCREATOR_UI_UIELEMENT_H
