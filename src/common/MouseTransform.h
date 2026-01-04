#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../modes/UiSharedResources.h"
#include "Shader.h"
#include "Texture.h"
#include "MapPoint.h"

class MouseTransform {
 public:
  MouseTransform(UiSharedResources& ui_shared_resources);

  void InitTransform(
    glm::vec3 translate = glm::vec3(0.0f),
    glm::quat rotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec3 scale = glm::vec3(1.0f)
    );

  void SetAxis(int key, bool mod_shift);

  void RotateSelected(double xpos, double ypos);

  void ScaleSelected(double xpos, double ypos);

  void TranslateSelected(double xpos, double ypos);

  void TranslateSelectedMapPoints(
    std::vector<MapPoint>& map_points, double xpos, double ypos);

  void TranslateSelectedMapPointsBack(std::vector<MapPoint>& map_points);

  void TranslateSelectedVerticesUp(double xpos, double ypos,
                                   const Texture& selection_mask, bool smooth);

  void Reset(
    glm::vec3 translate = glm::vec3(0.0f),
    glm::quat rotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec3 scale = glm::vec3(1.0f));

  void UpdateStartAngle();

  float UpdateAngle(float xpos, float ypos);

  glm::vec2 GetWorldOffset(float xpos, float ypos);

  glm::vec3 GetWorldOffsetTranslate(float xpos, float ypos);

  glm::vec3 axis_ = glm::vec3(1.0f);

  // what to set in case of Cancel()
  glm::vec3 prev_translate_ = glm::vec3(0.0f);
  glm::quat prev_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 prev_scale_ = glm::vec3(1.0f);

  // starts from prev_*, dynamically changed
  glm::vec3 cur_translate_ = glm::vec3(0.0f);
  glm::quat cur_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 cur_scale_ = glm::vec3(1.0f);

  float last_angle_ = 0.0f;
  float zero_angle_ = 0.0f;
  float zero_scale_length_ = 0.0f;
  float prev_value_y_ = 0.0f;

  UiSharedResources& ui_shared_resources_;

  Shader vertices_transform_shader_;

  double last_update_time_ = 0.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_
