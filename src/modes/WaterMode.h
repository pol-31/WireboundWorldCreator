#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void WaterModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void WaterModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void WaterModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class WaterMode final : public IEditMode {
 public:
  WaterMode(SharedResources& shared_resources);

  void Render() override;
  void RenderPicking() override;

  void Create(GLuint id);

  void Remove();

  /**
   generate 3 types: pond (take the highest point and fill everything below),
   river(inside the polygon generate pond, but the take bottom
   height of pond and for outside point if height less than
   pond bottom - reduce water height on difference), waterfall(fill only below)
   */
  /// currently only CPU
  // TODO: check: point idx in ccw order, convex polygons only
  void BakeWaterfall();
  void BakeRiver();
  void BakeLake();

  void BindCallbacks() override;

 protected:
  friend void WaterModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void WaterModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void WaterModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  void Init() {
      //TODO: idk - looks like this is picking...
    glGenVertexArrays(1, &points_vao_id_);
    glBindVertexArray(points_vao_id_);
    glGenBuffers(1, &points_buffer_id_);
    glBindBuffer(GL_ARRAY_BUFFER, points_buffer_id_);
    glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(GLuint), // TODO: 64 is max
                 nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);

    /// DUCK!
    //    glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(GLuint),
    //                          reinterpret_cast<void*>(0));
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint),
                           reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  std::vector<GLuint> points_data_;
  Shader points_shader_;
  GLuint points_buffer_id_{0};
  GLuint points_vao_id_{0};

  void RenderPoints() const {
    glBindVertexArray(points_vao_id_);
    glBindBuffer(GL_ARRAY_BUFFER, points_buffer_id_);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0, points_data_.size() * sizeof(GLuint),
                    points_data_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    points_shader_.Bind();

    glActiveTexture(GL_TEXTURE0);
//    height_map_.Bind();

    points_shader_.SetUniformVec4("color", 1, glm::value_ptr(colors::kWhite));
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, points_data_.size());
    if (points_data_.size() < 3) {
      return;
    }

    points_shader_.SetUniformVec4("color", 1, glm::value_ptr(colors::kBlue));
    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_LOOP, 0, points_data_.size());
  }


  /// MECHANIC which point to add or which to remove or how to render is here,
  /// while data stored in UiSlots. After both add and remove we leave the same order,
  /// so it's simply can be done in slots_. TODO: another question how to remove not from set
  /// of points but from graph with different in/out branches num ( do we need structure "graph"?
  /// - yes, obviously, so we have two UiSlots: points... idk

  // it's called only from vertex_mode_ and only from GLFW_MOUSE_LEFT + GLFW_PRESS
  void AddNewVertex(GLuint new_id) {
/*    if (cur_point_set_.empty()) {
      cur_point_set_.push_back({nullptr, {}, new_id});
      cur_selected_point_ = 0;
      return;
    }
    if (cur_selected_point_ == -1) {
      cur_selected_point_ = cur_point_set_[cur_point_set_.size() - 1].id;
    }
    if (cur_point_set_[cur_selected_point_].children.size() == 0) {
      cur_point_set_[cur_selected_point_].children.push_back({nullptr, {}, new_id});
    } else {
      auto next_child = cur_point_set_[cur_selected_point_].children[0];
      cur_point_set_[cur_selected_point_].children[0] = {nullptr, {}, new_id};
      cur_point_set_[cur_selected_point_].children[0].children.push_back(next_child);
    }*/
  }

  void AddNewObject() {
    if (vertex_mode_) {
      return; // we already here
    }
  }

  /// -1 as "not selected"
  GLuint cur_selected_point_{static_cast<GLuint>(-1)};

  bool vertex_mode_{false}; // vertex (polygon creation) or graph mode

  std::vector<Point> GenHeightMap() {
    //TODO: here need to take into account ALL points sets
    std::vector<Point> polygon(points_data_.size());
    for (int i = 0; i < polygon.size(); ++i) {
      polygon[i].x = points_data_[i] & 1023;
      polygon[i].y = points_data_[i] >> 10;
    }
    std::vector<Point> insidePoints;
    /*std::cout << "is convex: " << std::boolalpha << IsConvexPolygon(polygon)
              << std::noboolalpha << std::endl;
    for (int x = 0; x < 1024; x++) {
      for (int y = 0; y < 1024; y++) {
        Point p = {x, y};
        if (isInsideConvexPolygon(polygon, p)) {
          insidePoints.push_back(p);
        }
      }
    }*/
    return insidePoints;
  }

  //TODO: gpu
  /*void FloodFillPass() {
    int total_changed = 0;
    std::uint8_t cur_water_height, cur_terrain_height,
        near_water_height, near_terrain_height;
    for (int y = 1; y < 1023; ++y) {
      for (int x = 1; x < 1023; ++x) {
        int cur_idx = y * 1024 + x;
        cur_water_height = water_heights_[cur_idx];
        if (cur_water_height == 0) {
          continue;
        }
        cur_terrain_height = terrain_heights_[cur_idx];
        for (int i = -1; i <= 1; ++i) {
          for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
              continue;
            }
            int near_idx = cur_idx + i * 1024 + j;
            near_water_height = water_heights_[near_idx];
            near_terrain_height = terrain_heights_[near_idx];
            //
            GLuint new_water_height;
            if (cur_water_height > near_terrain_height) {
              if (cur_terrain_height < near_terrain_height) {
                new_water_height = cur_water_height;
              } else {
                new_water_height = near_terrain_height +
                                   (cur_water_height - cur_terrain_height);
                new_water_height = std::min(int(new_water_height), int(cur_water_height));
              }
              if (near_water_height < new_water_height) {
                ++total_changed;
                water_heights_[near_idx] =
                    static_cast<std::uint8_t>(new_water_height);
              }
              // TODO: we can add here "volume" and how high from starting point
            }
          }
        }

      }
    }
    std::cout << "total changed: " << total_changed << std::endl;
  }*/

  /**
   generate 3 types: pond (take the highest point and fill everything below),
   river(inside the polygon generate pond, but the take bottom
   height of pond and for outside point if height less than
   pond bottom - reduce water height on difference), waterfall(fill only below)
   */
  /*void FloodFill(const std::vector<Point>& water_points) {
    water_heights_.resize(terrain_heights_.size(), 0);
    for (auto p : water_points) {
      water_heights_[p.y * 1024 + p.x] = terrain_heights_[p.y * 1024 + p.x] + 10;
    }
    water_height_map_prev_total_sum_ = 0;
    int counter = 0;

    do {
      FloodFillPass();
      std::cout << "--- flood fill iteration# "
                << ++counter << std::endl;
    } while (!IsWaterHeightMapGenCompleted());
    water_height_map_.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
                 GL_UNSIGNED_BYTE, water_heights_.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }*/


  /*bool IsWaterHeightMapGenCompleted() {
    std::uint64_t new_sum{0};
    // (currently we use uint_8, but can use even uint size)
    // 1024 * 1024 * uint_size = 2^52 < 2^64 so we can use uint64_t
    for (auto i : water_heights_) {
      new_sum += i;
    }
    bool are_sums_equal = water_height_map_prev_total_sum_ == new_sum;
    water_height_map_prev_total_sum_ = new_sum;
    return are_sums_equal;
  }*/


  std::uint64_t water_height_map_prev_total_sum_{0};


  UiButton btn_bake_lake_;
  UiButton btn_bake_river_;
  UiButton btn_bake_waterfall_;
  UiButton btn_create_;
  UiButton btn_remove_;
  UiSlots slots_;

  using PointDataType = std::vector<ArbitraryGraph::Point>;
  PointDataType points_data_test_{};
  int edit_mode_selected_sample_id_test_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
