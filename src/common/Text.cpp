#include "Text.h"

namespace data {

// should be sorted by length locally for each sector, for prerender

const std::array<std::string_view, gTotalText> gText = {
    /// --- menu prerender ---
    "menu Terrain",
    "menu Water",
    "menu Roads",
    "menu Fences",
    "menu Placement",
    "menu Objects",
    "menu Biomes",
    "menu Tiles",
    "menu",
    "resolution",
    "show tips",
    "loading...",
    "accepted",
    "declined",
    "Do you want to exit?",
    "File exists, overwrite it?",
    "New file will clear all unsaved data, continue?",
    "Enter the path to load:",

    /// --- kTerrainMode ---
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
    "warp strength",
    "octave factor",
    "transform",
    "slopeness",
    "negative",
    "factors",
    "q",
    "r",
    "Perlin",
    "Cellular",
    "Metaballs",
    "FbmGrid",
    "FbmMulti",
    "FbmdPerlin",
    "FbmWarp",
    "FbmPerlinWarp",

    "erosion",
    "weathering",

    "tiling",
    "invert",
    "strength",

    /// --- kWaterMode ---
    "scale",
    "fetch",
    "spread blend",
    "swell",
    "peak enhancement",
    "short waves fade",
    "lambda",
    "layer 1",
    "layer 2",
    "layer 3",

    "wind speed",
    "wind angle",
    "sun X",
    "sun Y",
    "sun Z",
    "sun R",
    "sun G",
    "sun B",
};

}  // namespace data
