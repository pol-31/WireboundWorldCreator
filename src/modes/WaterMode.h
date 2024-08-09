#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_

#include "IEditMode.h"

class WaterMode final : public IEditMode {
 public:
  WaterMode(SharedResources& shared_resources)
      : IEditMode(shared_resources) {
    Init();
  }

  void Render() override {
    shared_resources_.tile_renderer.RenderPickingTerrain();
    shared_resources_.tile_renderer.Render();

    //TODO: here bind shader, vbo, vao, but... its all default static_sprite and rect vbo, so once
    shared_resources_.static_sprite_shader.Bind(); // TODO: or already bind? (don't think so)
//    vertex_mode_btn_.Bind(shared_resources_.static_sprite_shader);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    if (selected) {} // ... set appropriate color/brightness in shader

    RenderPoints();
  }

  void AddPoint(GLuint id) {
    if (points_data_.size() == 64) {
      std::cerr << "points overflow; rewriting last" << std::endl;
      points_data_.pop_back();
    }
    for (const auto& i : points_data_) {
      if (i == id) {
        return;
      }
    }
    std::cout << "Point id: " << id << std::endl;
    points_data_.push_back(id);
  }

  void ClearPoints() {
    points_data_.clear();
  }

  void BindCallbacks() override {}

 private:
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

  //  - we render water separately from terrain (we have different shaders)
  //  - we have few FloodFill algorithms: Pond/River/Waterfall
  //  - we have std::vector<std::vector<GLuint>> water_headers_; to draw
  //    multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)
  //  - we can remove/add new header, but after each such move we recalculate all water
  /**
   generate 3 types: pond (take the highest point and fill everything below),
   river(inside the polygon generate pond, but the take bottom
   height of pond and for outside point if height less than
   pond bottom - reduce water height on difference), waterfall(fill only below)
   */
  // TODO: check: point idx in ccw order, convex polygons only
  void BakeWaterfall();
  void BakeRiver();
  void BakeLake();

  bool IsConvex();

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

//  UiSlots::PointSetType cur_point_set_;

  /// -1 as "not selected"
  GLuint cur_selected_point_{static_cast<GLuint>(-1)};

  /// --- --- ui components --- ---
//  UiSlots slots_; // all graph data
//  UiSlider falloff_;
//  UiSlider radius_;

//  UiCollisionSprite add_new_btn_;
//  UiCollisionSprite remove_btn_;
//  UiCollisionSprite bake_waterflow_btn_;
//  UiCollisionSprite bake_river_btn_;
//  UiCollisionSprite bake_lake_btn_; // lake/pond/sea/ocean
//  UiCollisionSprite edit_position_btn_;
//  UiCollisionSprite edit_smooth_btn_;
//  UiCollisionSprite edit_slope_bottom_btn_;
//  UiCollisionSprite edit_slope_top_btn_;

  bool vertex_mode_{false}; // vertex (polygon creation) or graph mode

  // from deprecated Terrain.h:
  // --- --- --- --- --- --- --- ---
  // --- --- --- --- --- --- --- ---
  // --- --- --- --- --- --- --- ---
  // --- --- --- --- --- --- --- ---


  /* (180 degrees = surface; 90 degrees = straight down
   * (max - we have terrain-surface no Y-overlapp))
   * slope == 180	        | calm water
   * slope <180 && > 120 (30)   | river
   * slope < 120                | waterfall
   * */
  void Bake() {
    auto insidePoints = GenHeightMap();
    /// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
    //TODO: from points generate height map (!)
    // each triangle has its normal, so we assign water_type
    // for each triangle in shader
    // looks like we can just do this:
    // float mix_coef = mix(0.0f, 1.0f, dot(triangle_normal, world_up))
    // color = mix(black_color, blue_color, mix_coef);
    // speed = mix(0.0f, 30.0f, mix_coef);
  }

  struct Point {
    int x;
    int y;
  };

  // Function to calculate the cross product of vectors AB and AC
  int crossProduct(const Point& A, const Point& B, const Point& C) {
    int ABx = B.x - A.x;
    int ABy = B.y - A.y;
    int ACx = C.x - A.x;
    int ACy = C.y - A.y;
    return ABx * ACy - ABy * ACx;
  }
  // Function to check if the points in the vector are in CCW order
  bool isCCW(const std::vector<Point>& points) {
    int n = points.size();
    if (n < 3) {
      // A polygon must have at least 3 points
      return false;
    }

    for (int i = 0; i < n; ++i) {
      int j = (i + 1) % n;
      int k = (i + 2) % n;
      if (crossProduct(points[i], points[j], points[k]) <= 0) {
        return false;
      }
    }
    return true;
  }
  bool doIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2) {
    // Helper function to find the orientation of the ordered triplet (p, q, r)
    // 0 -> p, q and r are collinear
    // 1 -> Clockwise
    // 2 -> Counterclockwise
    auto orientation = [](const Point& p, const Point& q, const Point& r) {
      int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
      if (val == 0) return 0;
      return (val > 0) ? 1 : 2;
    };

    // Check if point q lies on segment pr
    auto onSegment = [](const Point& p, const Point& q, const Point& r) {
      if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
          q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) {
        return true;
      }
      return false;
    };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
      return true;

    // Special cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
  }
  bool isConvexPolygon(const std::vector<Point>& points) {
    int n = points.size();
    if (n < 3) {
      return false; // A polygon must have at least 3 points
    }

    // Check if the polygon is convex and in CCW order
    if (!isCCW(points)) {
      //      std::cout << "no ccw" << std::endl;
      return false;
    }

    // Check for intersections
    for (int i = 0; i < n; ++i) {
      for (int j = i + 2; j < n; ++j) {
        // Ignore adjacent edges and the first and last edge in a closed polygon
        if (i == 0 && j == n - 1) continue;

        if (doIntersect(points[i], points[(i + 1) % n], points[j], points[(j + 1) % n])) {
          return false;
        }
      }
    }

    return true;
  }
  // Function to check if the point p lies on the left side of the line segment from p1 to p2
  bool isLeft(Point p1, Point p2, Point p) {
    return (p2.x - p1.x) * (p.y - p1.y) - (p.x - p1.x) * (p2.y - p1.y) > 0;
  }
  // Function to check if a point lies inside a convex polygon
  bool isInsideConvexPolygon(const std::vector<Point>& polygon, Point p) {
    int n = polygon.size();
    if (n < 3) return false;
    for (int i = 0; i < n; i++) {
      Point p1 = polygon[i];
      Point p2 = polygon[(i + 1) % n];
      if (!isLeft(p1, p2, p)) {
        return false;
      }
    }
    return true;
  }

  /// looks like neither FloodFill nor GeightMap generation can be effective
  /// implemented in GPU, so let's compute its on CPU
  std::vector<Point> GenHeightMap() {
    std::vector<Point> polygon(points_data_.size());
    for (int i = 0; i < polygon.size(); ++i) {
      polygon[i].x = points_data_[i] & 1023;
      polygon[i].y = points_data_[i] >> 10;
    }
    std::vector<Point> insidePoints;
    std::cout << "is convex: " << std::boolalpha << isConvexPolygon(polygon)
              << std::noboolalpha << std::endl;
    for (int x = 0; x < 1024; x++) {
      for (int y = 0; y < 1024; y++) {
        Point p = {x, y};
        if (isInsideConvexPolygon(polygon, p)) {
          insidePoints.push_back(p);
        }
      }
    }
    return insidePoints;
    //    height_map_creation_shader_.Bind();
    //    glBindImageTexture(3, water_height_map_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    // ssbo already bind to binding=3
    //    glDispatchCompute(1024 / 32, 1024 / 32, 1);
    //    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  // TODO: do we need it in runtime or baking at cpu in advance is enough
  void FloodFIllGpuPass() {
    //    flood_fill_shader_.Bind();
    //    glBindImageTexture(0, height_map_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    //    glBindImageTexture(1, water_height_map_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    //    bool done = false;
    //    while (!done) {
    //      glDispatchCompute(1024 / 32, 1024 / 32, 1);
    //      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //      done = compareHeightMaps();
    //    }
  }
/*

  void FloodFillCPUPass() {
    int total_changed = 0;
    //TODO; can we rewrite it with compute shader?..
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
  }
*/

  // TODO:
  //  - we render water separately from terrain (we have different shaders)
  //  - we have few FloodFill algorithms: Pond/River/Waterfall
  //  - we have std::vector<std::vector<GLuint>> water_headers_; to draw
  //    multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)
  //  - we can remove/add new header, but after each such move we recalculate all water
  /**
   generate 3 types: pond (take the highest point and fill everything below),
   river(inside the polygon generate pond, but the take bottom
   height of pond and for outside point if height less than
   pond bottom - reduce water height on difference), waterfall(fill only below)
   */
/*
  void FloodFill(const std::vector<Point>& water_points) {
    water_heights_.resize(terrain_heights_.size(), 0);
    for (auto p : water_points) {
      water_heights_[p.y * 1024 + p.x] = terrain_heights_[p.y * 1024 + p.x] + 10;
    }
    water_height_map_prev_total_sum_ = 0;
    int counter = 0;

    do {
      //      FloodFIllGpuPass();
      FloodFillCPUPass();
      std::cout << "-------__--__---___-___--__--___-_flood fill iteration# "
                << ++counter << std::endl;
      //    } while (counter <= 100);
    } while (!IsWaterHeightMapGenCompleted());
    water_height_map_.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
                 GL_UNSIGNED_BYTE, water_heights_.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }


  bool IsWaterHeightMapGenCompleted() {
    //    std::vector<int> data(1024 * 1024);
    //    water_height_map_.Bind();
    //    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
    std::uint64_t new_sum{0};
    // (currently we use uint_8, but can use even uint size)
    // 1024 * 1024 * uint_size = 2^52 < 2^64 so we can use uint64_t
    for (auto i : water_heights_) {
      new_sum += i;
    }
    bool are_sums_equal = water_height_map_prev_total_sum_ == new_sum;
    water_height_map_prev_total_sum_ = new_sum;
    return are_sums_equal;
  }
  void InitFloodFill() {
    //    flood_fill_shader_ = Shader("../shaders/FloodFill.comp");
    GLuint water_tex_id;
    glGenTextures(1, &water_tex_id);
    glBindTexture(GL_TEXTURE_2D, water_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    water_height_map_ = Texture(water_tex_id, 1024, 1024, GL_RED);
  }*/

  std::uint64_t water_height_map_prev_total_sum_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_WATERMODE_H_
