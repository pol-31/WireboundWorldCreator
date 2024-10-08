#ifndef WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_

#include <string_view>

#include "../common/Texture.h"
#include "Tile.h"
#include "Visibility.h"
#include "../common/Paths.h"
#include "../common/Shader.h"

#include "../renderers/AllRenderers.h"

#include "../core/Map.h"

/// Render, RenderTerrain().. etc called from modes directly
class TileRenderer {
 public:
  explicit TileRenderer(const Paths& paths,
                        const Visibility& visibility,
                        Map& map);

  ~TileRenderer() {
    DeInitMapScaleUbo();
  }

  void SetTile(int tile_id) {
    //TODO: recompute surrounding water, etc...
  }

  //TODO: remove / refactor this pasta-macaroni
  Tile&  GetTile() {
    return cur_tile_;
  }

  void Render();

  void RenderPlacementDraw() const;

  void RenderPickingAll() const;

  /// see PlacementRenderer::Update() for the explanation
  void UpdatePlacement();

  //TODO: rewrite explanation (we duplicate it)

  /// not all modes needs all picking (for fence we don't need objects or
  /// other fences, while for water and terrain we need only terrain

  /// we place water, roads, fences, biomes, placement on PickingTerrain,
  /// so need only this function, but what about Objects?
  /// Objects can be placed on terrain, wter, feces and objects itself,
  /// so we have RenderPicking, that render these four (wrt each visibility
  /// mode)
  /// we don't always want to RenderPickingAll(), because for
  /// roads we also want to get terrain, so don't need RenderPickingObjects()

  //TODO 1: terrain idx 0-1024x1024, so water 1024x1024 - 2x1024x1024,
  // so fences 2x1024x1024-3x1024x1024 and objects 3x1024x1024-uint32_max
  //TODO 2: inline
  void RenderPickingTerrain() const;
  void RenderPickingWater() const;
  //  void RenderPickingRoads() const; // same as terrain
  void RenderPickingFences() const;
  //  void RenderPickingBiomes() const; // same as terrain
  void RenderPickingObjects() const;
  //  void RenderPickingPlacement() const; // same as terrain

  //TODO: should be called after each mode switching
  void ResetScale() {
    map_scale_ = 1.0f;
    UpdateScale();
  }

  // called from glfwCallback at mouse scroll (we don't need SetScale())
  void UpScale();
  void DownScale();

  [[nodiscard]] float GetScale() const {
    return map_scale_;
  }

  glm::vec3 GetPosition(int vertex_id);

 private:
  struct SurroundingWater {
    struct Unit {
      float height_water_;
      float height_diff_water_terrain_;
      // only "river" mechanic (not possible for waterfall/lake,
      // cause then we would need to draw it on both sides,
      // what is not really convenient, so let's do without it
      // so if you really want it on both tiles - draw it on both sides
      // reminder: if river, we pour with
    };
    // TODO: opt with k0_to_k1, k2_to_k3, so we don't need to store every pixel,
    //  but when if it's river then there's info loss
    std::vector<Unit> left;
    std::vector<Unit> right;
    std::vector<Unit> top;
    std::vector<Unit> bottom;
  };

  void InitMapScaleUbo();
  void DeInitMapScaleUbo();

  void UpdateScale();

  //TODO: holds cur_tile_ which is changing only at mode_Tiles
  // water data from adjacent left/right/above/below tiles
  //TODO: init & use at each tile loading and terrain::Update()
  SurroundingWater surrounding_water_;

  Tile cur_tile_{}; // TODO: changing only at tiles_mode

  const Visibility& visibility_;

  GLuint map_scale_ubo_{0};
  float map_scale_{1.0f};

  FencesRenderer fences_renderer_;
  ObjectsRenderer objects_renderer_;
  PlacementRenderer placement_renderer_;
  RoadsRenderer roads_renderer_;
  TerrainRenderer terrain_renderer_;
  WaterRenderer water_renderer_;

  Shader picking_shader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
