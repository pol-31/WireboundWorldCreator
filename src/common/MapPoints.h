#ifndef WIREBOUNDWORLDCREATOR_MAPPOINTS_H
#define WIREBOUNDWORLDCREATOR_MAPPOINTS_H

#include <glad/glad.h>

#include "models/ModelManager.h"
#include "Shader.h"
#include "Texture.h"
#include "MapPoint.h"

class MapPoints {
  public:
  MapPoints(ModelManager& mdl_manager);

  ~MapPoints() {
    DeInit();
  }

  void SetData(std::vector<MapPoint>* map_points,
    std::vector<glm::uvec2>* map_joints);

  void AddJoints(GLuint id);

  std::vector<MapPoint>* GetPoints() {
    return map_points_;
  }

  void UpdateJointsBuffer();

  void RenderPoints(glm::vec4 color);

  void RenderPickingPoints();

  void RenderJoints(const Texture& hmap, float map_scale);

  void AddPoint(MapPoint point);

 private:
  void Init();

  void DeInit();

  GLuint vao_joints_ = 0;
  GLuint vbo_joints_ = 0;
  Shader shader_joints_;
  ModelManager& mdl_manager_;

  std::vector<MapPoint>* map_points_ = nullptr;
  std::vector<glm::uvec2>* map_joints_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_MAPPOINTS_H
