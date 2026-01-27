#include "MapPoints.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Details.h"

MapPointsBase::MapPointsBase(ModelManager& mdl_manager)
    : mdl_manager_(mdl_manager),
      shader_joints_("../shaders/MapJoint.vert", "../shaders/MapJoint.frag") {
  Init();
}

void MapPointsBase::AddJoints(GLuint id) {
  if (!map_points_ || !map_joints_) {
    return;
  }
  // point id == terrain_id; so for joints we should subtract
  id -= details::kIdOffsetObjects;
  for (int i = 0; i < map_points_->size(); ++i) {
    if (!(*map_points_)[i].selected) {
      continue;
    }
    bool joint_exists = false;
    GLuint point_id = i;
    for (auto joint : *map_joints_) {
      if (joint == glm::uvec2{point_id, id} ||
          joint == glm::uvec2{id, point_id}) {
        joint_exists = true;
        break;
      }
    }
    if (!joint_exists) {
      map_joints_->push_back({point_id, id});
    }
  }
  UpdateJointsBuffer();
}

void MapPointsBase::UpdateJointsBuffer() {
  if (!map_points_ || !map_joints_) {
    return;
  }
  /// 4 points per joint
  std::vector<glm::vec2> buffer(map_joints_->size() * 4);
  for (int i = 0; i < map_joints_->size(); ++i) {
    GLuint id1 = (*map_joints_)[i].x;
    auto position1 = (*map_points_)[id1].position;
    GLuint id2 = (*map_joints_)[i].y;
    auto position2 = (*map_points_)[id2].position;
    /// camera-faced quads
    buffer[i * 4] = position1;
    buffer[i * 4 + 1] = position1 + glm::vec2(0.1);
    buffer[i * 4 + 2] = position2 + glm::vec2(0.1);
    buffer[i * 4 + 3] = position2;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo_joints_);
  glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(glm::vec2),
               buffer.data(), GL_DYNAMIC_DRAW);
}

void MapPointsBase::RenderJoints(const Texture& hmap, float map_scale,
                                 glm::vec4 color) {
  if (!map_joints_) {
    return;
  }
  auto map_model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  shader_joints_.Bind();
  glUniform1i(0, 0);
  glUniform4fv(2, false, glm::value_ptr(color));
  glActiveTexture(GL_TEXTURE0);
  hmap.Bind();
  glUniformMatrix4fv(1, 1, false, glm::value_ptr(map_model));
  glBindVertexArray(vao_joints_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, map_joints_->size() * 4);
}

void MapPointsBase::Init() {
  glGenVertexArrays(1, &vao_joints_);
  glBindVertexArray(vao_joints_);
  glGenBuffers(1, &vbo_joints_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_joints_);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(glm::vec2), 0);
  glBindVertexArray(0);
}

void MapPointsBase::DeInit() {
  glDeleteBuffers(1, &vbo_joints_);
  glDeleteVertexArrays(1, &vao_joints_);
}

MapPoints::MapPoints(ModelManager& mdl_manager) : MapPointsBase(mdl_manager) {}

void MapPoints::AddPoint(GLuint pos_id) {
  if (map_points_) {
    map_points_->push_back(
        {glm::vec2{pos_id >> 10, pos_id & 1023} / 16.0f - 32.0f, false});
  }
}

void MapPoints::SetData(std::vector<MapPoint>* map_points,
                        std::vector<glm::uvec2>* map_joints) {
  map_points_ = map_points;
  map_joints_ = map_joints;
}

void MapPoints::RenderPoints(glm::vec4 color) {
  if (map_points_) {
    mdl_manager_.RenderMapPoints(*map_points_, color);
  }
}

void MapPoints::RenderPickingPoints() {
  if (map_points_) {
    mdl_manager_.RenderPickingMapPoints(*map_points_);
  }
}

MapObjects::MapObjects(ModelManager& mdl_manager)
    : MapPointsBase(mdl_manager) {}

void MapObjects::AddPoint(GLuint pos_id) {
  if (map_points_) {
    map_points_->push_back(
        {glm::vec2{pos_id >> 10, pos_id & 1023} / 16.0f - 32.0f, false});
    rotates_->emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
    scales_->emplace_back(1.0f, 1.0f, 1.0f);
  }
}

void MapObjects::SetData(std::vector<MapPoint>* map_points,
                         std::vector<glm::uvec2>* map_joints,
                         std::vector<glm::quat>* rotates,
                         std::vector<glm::vec3>* scales) {
  map_points_ = map_points;
  map_joints_ = map_joints;
  rotates_ = rotates;
  scales_ = scales;
}

void MapObjects::RenderPoints(glm::vec4 color) {
  if (map_points_) {
    mdl_manager_.RenderMapPoints(*map_points_, *rotates_, *scales_, color);
  }
}

void MapObjects::RenderPickingPoints() {
  if (map_points_) {
    mdl_manager_.RenderPickingMapPoints(*map_points_, *rotates_, *scales_);
  }
}
