#ifndef WIREBOUNDDEV_SRC_TERRAIN_H_
#define WIREBOUNDDEV_SRC_TERRAIN_H_

#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Colors.h"

//TODO: need to check this: height_map_ is GL_RGBA && GL_UNSIGNED_BYTE / INT

class Terrain {
 public:
  Terrain(
      std::string_view shader_vert_path,
      std::string_view shader_frag_path,
      std::string_view height_map_path,
      std::string_view tex_material_path,
      std::string_view tex_occlusion_path,
      std::string_view shader_vert_pick_path,
      std::string_view shader_frag_pick_path)
      : shader_(shader_vert_path, shader_frag_path),
        shader_picking_(shader_vert_pick_path, shader_frag_pick_path),
        height_map_(height_map_path),
        tex_color_(tex_material_path),
        tex_occlusion_(tex_occlusion_path) {
    Init();
  }

  ~Terrain() {
    DeInitHeightMapCreation();
    // TODO: that's not all
  }

  void SetScale(float scale) {
    scale_ = scale;
    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(scale_, scale_, scale_));
    shader_.Bind();
    shader_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));
    shader_picking_.Bind();
    shader_picking_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));
    points_shader_.Bind();
    points_shader_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));
  }

  [[nodiscard]] float GetScale() const {
    return scale_;
  }

  void SetId(GLuint id) {
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
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
    points_data_.push_back(id);
//    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
//                    points_data_.size() * sizeof(GLuint),
//                    points_data_.data());
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  GLuint GetHeightId(double cursor_pos_x, double cursor_pos_y) {
    RenderPicking();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_picking_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    GLuint id;
    glReadPixels(static_cast<int>(cursor_pos_x),
                 details::kWindowHeight - static_cast<int>(cursor_pos_y), 1, 1,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return id;
  }

  void RenderPicking() {
    shader_picking_.Bind();
    glBindVertexArray(vao_);

    glActiveTexture(GL_TEXTURE0);
    height_map_.Bind();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_picking_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindVertexArray(0);
  }

  void Render() const {
    shader_.Bind();
    glBindVertexArray(vao_);

    glActiveTexture(GL_TEXTURE0);
    height_map_.Bind();

    glActiveTexture(GL_TEXTURE1);
    tex_color_.Bind();

    glActiveTexture(GL_TEXTURE2);
    tex_occlusion_.Bind();

    shader_.SetUniformVec4("blend_color", 1, glm::value_ptr(colors::kWhite));

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
    if (water_height_map_.GetId() != 0) {
      glActiveTexture(GL_TEXTURE0);
      water_height_map_.Bind();
      shader_.SetUniformVec4("blend_color", 1, glm::value_ptr(colors::kBlue));
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
    }
    glBindVertexArray(0);

    RenderPoints();
    RenderWater();
  }

  void RenderWater() const {
/*    for (const auto& water : all_separate_water) {
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_water_points_);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                      water.size() * sizeof(float), water.data());
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      glDrawArrays(GL_POINTS, 0, water.size());
      //TODO: from each point we render triangle in geom shader?
      // SO it looks like we need: tessellation for borders (randomized
      //  cubic spline), tessellation for waves (quads);
      //  DO WE need geometry shader to create triangles out of points,
      //  or we can do this with patches in tessellation shader?
    }*/
  }

  /* (180 degrees = surface; 90 degrees = straight down
   * (max - we have terrain-surface no Y-overlapp))
   * slope == 180	        | calm water
   * slope <180 && > 120 (30)   | river
   * slope < 120                | waterfall
   * */
  void Bake() {
    auto insidePoints = GenHeightMap();
    /// here water_height_map_ already contains either 0 or actual height
    FloodFill(insidePoints);
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

  void ClearPoints() {
    points_data_.clear();
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
//    GLuint zero = 0;
//    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F,
//                      GL_RGBA, GL_FLOAT, &zero);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void RenderPoints() const {
    glBindVertexArray(points_vao_id_);
    glBindBuffer(GL_ARRAY_BUFFER, points_buffer_id_);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0, points_data_.size() * sizeof(GLuint),
                    points_data_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    points_shader_.Bind();

    glActiveTexture(GL_TEXTURE0);
    height_map_.Bind();

    points_shader_.SetUniformVec4("color", 1, glm::value_ptr(colors::kWhite));
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, points_data_.size());
    if (points_data_.size() < 3) {
      return;
    }

    points_shader_.SetUniformVec4("color", 1, glm::value_ptr(colors::kBlue));
    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_LOOP, 0, points_data_.size());

//    glMapBufferRange();// TODO: learn

//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_debug_points_id_);
//    void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 2*sizeof(GLuint), GL_MAP_READ_BIT);
//    auto data = reinterpret_cast<glm::uvec2*>(ptr);
//    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//    std::cout << data->x << " and " << data-> y << std::endl;
  }

  //TODO: tile_id unused by now, but when we have multiple tiles, we need it
  glm::vec3 GetPosition(int tile_id, int coord_x, int coord_z) {
    /// on surface (no height)
    auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
//    where.y *= dmap_depth_;
    where.y /= 64.0f; // TODO: idk why 64.0f
//    std::cout << where.y << std::endl;
    return where;
  }

 private:
  void Init() {
    water_caustics_ = Texture("../assets/water_caustics.png");
    shader_.Bind();
    shader_.SetUniform("tex_displacement", 0);
    shader_.SetUniform("tex_color", 1);
    shader_.SetUniform("tex_occlusion", 2);
    shader_.SetUniform("dmap_depth", dmap_depth_);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(scale_, scale_, scale_));
    shader_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));

    shader_picking_.Bind();
    shader_picking_.SetUniform("tex_displacement", 0);
    shader_picking_.SetUniform("dmap_depth", dmap_depth_);
    shader_picking_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));


    points_shader_ = Shader("../shaders/PointsPolygon.vert",
                            "../shaders/PointsPolygon.frag");
    points_shader_.Bind();
    points_shader_.SetUniform("tex_displacement", 0);
    points_shader_.SetUniform("dmap_depth", dmap_depth_);
    points_shader_.SetUniformMat4v("transform", 1, false, glm::value_ptr(transform));

    // Define the vertices (not actually used, but required for the draw call)
    GLfloat vertices[] = {
        -0.5f, 0.0f, -0.5f, 1.0f,
        0.5f, 0.0f, -0.5f, 1.0f,
        -0.5f, 0.0f,  0.5f, 1.0f,
        0.5f, 0.0f,  0.5f, 1.0f
    }; // TODO: its useless

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void*>(0));

    InitHeights();
    InitPicking();

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

//    glGenBuffers(1, &ssbo_points_);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
//    glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * sizeof(GLuint),
//                 nullptr, GL_DYNAMIC_DRAW);
//    GLuint zero = 0;
//    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F,
//                      GL_RGBA, GL_FLOAT, &zero);
//    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_points_);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//    glGenBuffers(1, &ssbo_debug_points_id_);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_debug_points_id_);
//    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * sizeof(GLuint),
//                 nullptr, GL_DYNAMIC_DRAW);
//    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_debug_points_id_);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    InitFloodFill();
    InitHeightMapCreation();
  }

  void InitPicking() {
    glGenFramebuffers(1, &fbo_picking_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_picking_);

    GLuint tex_picking_id;
    glGenTextures(1, &tex_picking_id);
    glBindTexture(GL_TEXTURE_2D, tex_picking_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, details::kWindowWidth,
                 details::kWindowHeight, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    tex_picking_ = Texture(tex_picking_id, details::kWindowWidth,
                           details::kWindowHeight, GL_R32UI); // TODO: not sure about format

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_picking_.GetId(), 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void InitHeights() {
    // TODO: get height, width and only then init....
    terrain_heights_.resize(1024 * 1024);
    height_map_.Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                  terrain_heights_.data());
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
  }

  void InitHeightMapCreation() {
//    height_map_creation_shader_ = Shader("../shaders/GenHeightMap.comp");
  }

  void DeInitHeightMapCreation() {
//    glDeleteBuffers(1, &polygonBuffer);
  }

  const Texture& GetHeightMap() const {
    return height_map_;
  }
  float GetDmapDepth() const {
    return dmap_depth_;
  }

 private:
  Shader shader_;
  Shader shader_picking_;

  Texture height_map_;

  Texture tex_color_;
  Texture tex_occlusion_;

  GLuint fbo_picking_; //TODO: RAII wrapper
  Texture tex_picking_;

  GLuint vao_{0};
  GLuint vbo_{0};

  float scale_{1.0f};

  /// need to duplicate both on GPU and CPU:
  /// for gpu to render terrain; for cpu to dynamically obtain object y pos
  std::vector<uint8_t> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  //
  float dmap_depth_ = 256.0f; // height_map_ scale

//  GLuint ssbo_points_{0};
  std::vector<GLuint> points_data_;

  Shader points_shader_;
  GLuint points_buffer_id_{0};
  GLuint points_vao_id_{0};

  Texture water_caustics_;

  Texture water_height_map_;

//  GLuint ssbo_debug_points_id_{0};


  std::uint64_t water_height_map_prev_total_sum_{0};

//  Shader height_map_creation_shader_;
//  Shader flood_fill_shader_;
};

#endif  // WIREBOUNDDEV_SRC_TERRAIN_H_
