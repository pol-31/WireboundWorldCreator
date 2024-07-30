#include "UiCore.h"

//TODO: it's not necessary to make them global; we can avoid it by now

TerrainMode gTerrainMode;
WaterMode gWaterMode;
RoadsMode gRoadsMode;
FencesMode gFencesMode;
BiomesMode gBiomesMode;
ObjectsMode gObjectsMode;
HeightMapDrawMode gHeightMapDrawMode;

IGenMode* gCurMode = &gTerrainMode;
Visibility gVisibility;
