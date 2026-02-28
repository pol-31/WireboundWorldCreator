#ifndef WIREBOUNDWORLDCREATOR_MODETERRAIN_H
#define WIREBOUNDWORLDCREATOR_MODETERRAIN_H

class ModeTerrain {
 public:
  void CreateUi() {
    auto base = new UiBase;
    base->AddSprite(new UiSprite(data::VboIdMain::kTerrainTerrainMode));
    base->AddButton(new UiSprite(data::VboIdMain::kTerrainUpdate,
                                 [this] { ui_bake_.Bake(1000, 1000, 0.1f); }));
    base->AddComplex(new UiSlots(params));
  }
};

#endif  // WIREBOUNDWORLDCREATOR_MODETERRAIN_H
