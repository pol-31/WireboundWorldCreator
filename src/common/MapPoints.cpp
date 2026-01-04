#include "MapPoints.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Details.h"

MapPoints::MapPoints(ModelManager& mdl_manager)
    : mdl_manager_(mdl_manager),
      shader_joints_("../shaders/MapJoint.vert", "../shaders/MapJoint.frag") {
  Init();
}

void MapPoints::SetData(std::vector<MapPoint>* map_points,
    std::vector<glm::uvec2>* map_joints) {
  map_points_ = map_points;
  map_joints_ = map_joints;
}

void MapPoints::AddJoints(GLuint id) {
  if (!map_points_ || !map_joints_) {
    return;
  }
  for (int i = 0; i < map_points_->size(); ++i) {
    if (!(*map_points_)[i].selected) {
      continue;
    }
    bool joint_exists = false;
    GLuint point_id = details::kIdOffsetObjects + 100 + i;
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

void MapPoints::UpdateJointsBuffer() {
  if (!map_points_ || !map_joints_) {
    return;
  }
  /// 4 points per joint
  std::vector<GLuint> buffer(map_joints_->size() * 4);
  for (int i = 0; i < map_joints_->size(); ++i) {
    GLuint id1 = (*map_joints_)[i].x - details::kIdOffsetObjects - 100;
    auto position1 = (*map_points_)[id1].position;
    GLuint id2 = (*map_joints_)[i].y - details::kIdOffsetObjects - 100;
    auto position2 = (*map_points_)[id2].position;
    /// camera-faced quads
    buffer[i * 4] = position1;
    buffer[i * 4 + 1] = position1 + 1;
    buffer[i * 4 + 2] = position2 + 1;
    buffer[i * 4 + 3] = position2;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo_joints_);
  glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLuint), buffer.data(),
               GL_DYNAMIC_DRAW);
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

void MapPoints::RenderJoints(
    const Texture& hmap, float map_scale) {
  if (!map_joints_) {
    return;
  }
  auto map_model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  shader_joints_.Bind();
  glUniform1i(0, 0);
  glActiveTexture(GL_TEXTURE0);
  hmap.Bind();
  glUniformMatrix4fv(1, 1, false, glm::value_ptr(map_model));
  glBindVertexArray(vao_joints_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, map_joints_->size() * 4);
}

void MapPoints::Init() {
  glGenVertexArrays(1, &vao_joints_);
  glBindVertexArray(vao_joints_);
  glGenBuffers(1, &vbo_joints_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_joints_);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);
  glBindVertexArray(0);
}

void MapPoints::DeInit() {
  glDeleteBuffers(1, &vbo_joints_);
  glDeleteVertexArrays(1, &vao_joints_);
}

void MapPoints::AddPoint(MapPoint point) {
  if (map_points_) {
    map_points_->push_back(point);
  }
}
