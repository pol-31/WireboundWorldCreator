#include "Text.h"

namespace data {

// should be sorted by length locally for each sector, for prerender

const std::array<std::string_view, gTotalText> gText = {
    "scale",
    "seed",
    "jitter",
    "phase",
    "phases",
    "octaves",
    "shift",
    "time shift",
    "gain",
    "lacunarity",
    "translate",
    "warp strength",
    "octave factor",
    "transform",
    "slopeness",
    "negative",
    "factors",
    "q",
    "r",
    "erosion",
    "weathering",
    "menu Terrain",
    "menu Water",
    "menu Roads",
    "menu Fences",
    "menu Placement",
    "menu Objects",
    "menu Biomes",
    "menu Tiles",
    "menu",
    "resolution"
    "show tips"
    "New game",
    "Load game",
    "Continue",
    "Archive",
    "Options",
    "Credits",
    "Quit",
    "Loading",
    "Copyright",
    "https",
    "github",
    "com",
    "Superman",
    "Lamb of God",
    "Pantera",
    "Hatebreed",
};

} // namespace text
