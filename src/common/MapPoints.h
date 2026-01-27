#ifndef WIREBOUNDWORLDCREATOR_MAPPOINTS_H
#define WIREBOUNDWORLDCREATOR_MAPPOINTS_H

#include <glad/glad.h>

#include "MapPoint.h"
#include "Shader.h"
#include "Texture.h"
#include "models/ModelManager.h"

class MapPointsBase {
 public:
  MapPointsBase(ModelManager& mdl_manager);

  ~MapPointsBase() { DeInit(); }

  void AddJoints(GLuint id);

  std::vector<MapPoint>* GetPoints() { return map_points_; }

  void UpdateJointsBuffer();

  void RenderJoints(const Texture& hmap, float map_scale, glm::vec4 color);

 protected:
  void Init();

  void DeInit();

  GLuint vao_joints_ = 0;
  GLuint vbo_joints_ = 0;
  Shader shader_joints_;
  ModelManager& mdl_manager_;

  std::vector<MapPoint>* map_points_ = nullptr;
  std::vector<glm::uvec2>* map_joints_ = nullptr;
};

class MapPoints final : public MapPointsBase {
 public:
  MapPoints(ModelManager& mdl_manager);

  void SetData(std::vector<MapPoint>* map_points,
               std::vector<glm::uvec2>* map_joints);

  void RenderPoints(glm::vec4 color);

  void RenderPickingPoints();

  void AddPoint(GLuint pos_id);
};

class MapObjects final : public MapPointsBase {
 public:
  MapObjects(ModelManager& mdl_manager);

  void SetData(std::vector<MapPoint>* map_points,
               std::vector<glm::uvec2>* map_joints,
               std::vector<glm::quat>* rotates, std::vector<glm::vec3>* scales);

  void RenderPoints(glm::vec4 color);

  void RenderPickingPoints();

  void AddPoint(GLuint pos_id);

  std::vector<glm::quat>* GetRotates() { return rotates_; }

  std::vector<glm::vec3>* GetScales() { return scales_; }

 private:
  std::vector<glm::quat>* rotates_ = nullptr;
  std::vector<glm::vec3>* scales_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_MAPPOINTS_H
