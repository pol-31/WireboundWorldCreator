#include "UiElement.h"

void UiElement::Add(UiElement *inElement) {
  inElement->mParent = this;
  mChildren.push_back(inElement);
}

void UiElement::Clear() {
  for (UiElement *e : mChildren) delete e;
}

bool UiElement::Contains(int inX, int inY) const {
  return false;  // todo; aabb
}

bool UiElement::ContainsWidened(int inX, int inY, int inBorder) const {
  return false;
}

UiElement *UiElement::FindByID(int id) {
  if (static_cast<data::UiId>(id) == id_) {
    return this;
  }
  for (UiElement *e : mChildren) {
    UiElement *element = e->FindByID(id);
    if (element != nullptr) return element;
  }
  return nullptr;
}

void UiElement::AutoLayout() {
  for (UiElement *e : mChildren) {
    e->AutoLayout();
    // todo; set position based on alignment, width, padding
    //  probably after our old UpdateTransform()
  }
}

void UiElement::UpdateDbgTransform() {
  throw "UiElement::UpdateDbgTransform() not implemented";
}

void UiElement::Update(float delta_time, UiLayer &ui_layer, glm::vec2 translate,
                       float rotate, glm::vec2 scale) {
  translate += position_;
  rotate += rotate_;
  scale *= scale_;
  for (UiElement *e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}
