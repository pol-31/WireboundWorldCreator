#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_IGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_IGRAPH_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "FixedSizeQueue.h"

class IGraph {
 public:
  struct BaseInstanceData {
    std::string name;
    glm::vec4 color;
    bool do_show;

    /// terrain: vertical layer
    /// fences: wooden, chain-link, picket
    /// roads: soil, asphalt, gravel
    /// water: river, waterfall, pond
    /// biomes: any possible created from params
    int type_id;
  };

  virtual void CreateGraph() = 0;
  virtual void SelectGraph(int id) = 0;

  // returns if smt was removed
  virtual bool RemoveGraph(int id) = 0;
  virtual void Render(glm::vec2 mouse_pos) = 0;

  virtual int GetSize() const noexcept = 0;
  virtual int GetSlotId() const noexcept = 0;

  virtual void Release() {}

  virtual BaseInstanceData* GetBaseInstanceData(int id) = 0;

  virtual const std::string* GetNameRef(int instance_id) const = 0;

  virtual std::string* GetNameRef(int instance_id) = 0;

  // "Arbitrary" do have them, "Terrain Grid" may not
//  Create/Select/Remove vertex;
//  Create/Select/Remove edge;
//  Create/Select/Remove face;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_IGRAPH_H_
