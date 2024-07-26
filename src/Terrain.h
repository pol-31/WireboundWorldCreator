#ifndef WIREBOUNDDEV_SRC_TERRAIN_H_
#define WIREBOUNDDEV_SRC_TERRAIN_H_

#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Colors.h"

class Terrain {
 public:
  struct PointData {
    float x{0};
    float y{0};
    float z{0};
    float id{0};
  };

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

  void SetId(PointData point_data) {
    if (points_data_.size() == 64) {
      std::cerr << "points overflow; rewriting last" << std::endl;
      points_data_.pop_back();
    }
    for (const auto& i : points_data_) {
      if (i.x == point_data.x &&
          i.y == point_data.y &&
          i.z == point_data.z &&
          i.id == point_data.id) {
        return;
      }
    }
    std::cout
        << "Position: " << point_data.x
        << ' ' << point_data.y << ' ' << point_data.z
        << "; Id: " << point_data.id << std::endl;
    shader_.Bind();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
    points_data_.push_back(point_data);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    points_data_.size() * sizeof(PointData),
                    points_data_.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  PointData GetHeightId(double cursor_pos_x, double cursor_pos_y) {
    RenderPicking();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_picking_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    PointData point_data;
    glReadPixels(static_cast<int>(cursor_pos_x),
                 details::kWindowHeight - static_cast<int>(cursor_pos_y), 1, 1,
                 GL_RGBA, GL_FLOAT, &point_data);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return point_data;
  }

  void RenderPicking() {
    shader_picking_.Bind();
    glBindVertexArray(vao_);

    glActiveTexture(GL_TEXTURE0);
    height_map_.Bind();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_picking_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 64 * 64);
    //    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
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

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 64 * 64);
//    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
    glBindVertexArray(0);

    RenderPoints();
  }

  void Bake() {
    for (const auto& point : points_data_) {
      // get average?
    }
  }

  void ClearPoints() {
    points_data_.clear();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
    PointData zero;
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F,
                      GL_RGBA, GL_FLOAT, &zero);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void RenderPoints() const {
    glBindVertexArray(points_vao_id_);
    glBindBuffer(GL_ARRAY_BUFFER, points_buffer_id_);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0, points_data_.size() * sizeof(PointData),
                    points_data_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    points_shader_.Bind();
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

  //TODO: tile_id unused by now, but when we have multiple tiles, we need it
  glm::vec3 GetPosition(int tile_id, int coord_x, int coord_z) {
    /// on surface (no height)
    auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(heights_[coord_x + coord_z * 1024]);
//    where.y *= dmap_depth_;
    where.y /= 64.0f; // TODO: idk why 64.0f
//    std::cout << where.y << std::endl;
    return where;
  }

 private:
  void Init() {
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
    glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(PointData), // TODO: 64 is max
                 nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ssbo_points_);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_points_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * sizeof(PointData),
                 nullptr, GL_DYNAMIC_DRAW);
    PointData zero;
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F,
                      GL_RGBA, GL_FLOAT, &zero);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_points_);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void InitPicking() {
    glGenFramebuffers(1, &fbo_picking_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_picking_);

    GLuint tex_picking_id;
    glGenTextures(1, &tex_picking_id);
    glBindTexture(GL_TEXTURE_2D, tex_picking_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, details::kWindowWidth,
                 details::kWindowHeight, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    tex_picking_ = Texture(tex_picking_id, details::kWindowWidth,
                           details::kWindowHeight, GL_RGBA32F); // TODO: not sure about format

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_picking_.GetId(), 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void InitHeights() {
    // TODO: get height, width and only then init....
    heights_.resize(1024 * 1024);
    height_map_.Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, heights_.data());
    glBindTexture(GL_TEXTURE_2D, 0);
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
  std::vector<uint8_t> heights_;

  //
  float dmap_depth_ = 16.0f; // height_map_ scale

  GLuint ssbo_points_{0};
  std::vector<PointData> points_data_;

  Shader points_shader_;
  GLuint points_buffer_id_{0};
  GLuint points_vao_id_{0};
};

#endif  // WIREBOUNDDEV_SRC_TERRAIN_H_
