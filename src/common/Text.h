#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_

#include <array>
#include <string>

namespace text {

enum class Id {
  kMenuTerrain,
  kMenuWater,
  kMenuRoads,
  kMenuFences,
  kMenuPlacement,
  kMenuObjects,
  kMenuBiomes,
  kMenuTiles,
  kMenu,

  Newgame,
      Loadgame,
          Continue,
              Archive,
                  Options,
                      Credits,
                          Quit,
                              Loading,
                                  Copyright,
                                      https,
                                          github,
                                              com,
                                                  Superman,
                                                      LambofGod,
                                                          Pantera,
                                                              Hatebreed,
  kTotal,
  kNone, // shouldn't be used (always check != kNone)
};

inline constexpr int gTotalText = static_cast<int>(Id::kTotal)
                                  - static_cast<int>(Id::kMenuTerrain);

extern const std::array<std::string_view, gTotalText> gText;

} // namespace text

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
