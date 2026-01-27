#include "Text.h"

namespace data {

// should be sorted by length locally for each sector, for prerender

const std::array<std::string_view, gTotalText> gText = {
    "TERRAIN",
    "WATER",
    "PLACEMENT",
    "OBJECTS",
    "BIOMES",
    "TILES",
    "PLAYER",
    "MENU",

    "resolution",
    "tips",

    "loading",
    "accept",
    "decline",
    "Are you sure you want to exit?",
    "overwrite",
    "load",
    "open",

    /// --- kTerrainRender prerender ---
    "scale",
    "scale X",
    "scale Y",
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
    "warp",
    "octave",
    "transform",
    "slopeness",
    "negative",
    "factors",
    "q",
    "r",
    "perlin",
    "cellular",
    "metaballs",
    "fbm grid",
    "fbm multi",
    "fbmd perlin",
    "fbm warp",
    "fbm perlin warp",

    "erosion",
    "weathering",

    "tiling",
    "invert",
    "strength",

    /// --- kWaterMode prerender ---
    "scale",
    "fetch",
    "spread",
    "swell",
    "peaks",
    "short waves",
    "lambda",
    "1st layer",
    "2nd layer",
    "3rd layer",
    "transparency",
    "viscosity",
    "curvature",
    "heights",
    "raise",
    "sagging",
    "radius",
    "flattened",

    /// --- kObjectsMode ---
    "health",
    "speed",
    "attack",
    "attack speed",

    /// --- kBiomeMode prerender ---
    "wind angle",
    "wind speed",
    "sun offset x",
    "sun offset y",
    "sun offset z",
    "sun color r",
    "sun color g",
    "sun color b",

    /// --- kPlacementMode ---
    "curvature",
    "heights",
    "raise",
    "sagging",
    "radius",
    "flattened",
};

}  // namespace data
