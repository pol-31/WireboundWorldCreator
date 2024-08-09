#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_

#include <glm/glm.hpp>

#include "../core/Tile.h"
#include "../common/Paths.h"

class TerrainRenderer {
 public:
  TerrainRenderer(Tile& tile, const Paths& paths)
      : tile_(tile),
        shader_(paths.shader_terrain_vert, paths.shader_terrain_frag) {
    Init();
  }

  void Render() const {
    if(glfwGetKey(gWindow, GLFW_KEY_1)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //TODO; we still need tessellation - we want to see how it should be in game
    //TODO: render terrain here

    shader_.Bind();
    glBindVertexArray(vao_);

    glActiveTexture(GL_TEXTURE0);
    tile_.map_terrain_height.Bind();

//    glActiveTexture(GL_TEXTURE1);
//    tex_color_.Bind();

    glActiveTexture(GL_TEXTURE2);
    tile_.map_terrain_occlusion.Bind();

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
    /*if (water_height_map_.GetId() != 0) {
      glActiveTexture(GL_TEXTURE0);
      water_height_map_.Bind();
      shader_.SetUniformVec4("blend_color", 1, glm::value_ptr(colors::kBlue));
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
    }*/
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void RenderPicking() const {
//    picking_fbo_.Bind();
//    //TODO: render terrain here
//    shader_picking_.Bind();
//    glBindVertexArray(vao_);
//    glActiveTexture(GL_TEXTURE0);
//    height_map_.Bind();
//    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
//    glBindVertexArray(0);
  }

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const {
    int coord_x = vertex_id & 1023;
    int coord_z = vertex_id >> 10;/*
    auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
    where.y /= 64.0f; // TODO: idk why 64.0f*/
//    return where;
    return {};
  }

 private:
  void Init() {
//    return; //TODO: we use not "empty" but fill with tex_coords and pos !!!
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

    shader_.Bind();
    shader_.SetUniform("tex_displacement", 0);
    shader_.SetUniform("tex_color", 1);
    shader_.SetUniform("tex_occlusion", 2);

//    shader_picking_.Bind();
//    shader_picking_.SetUniform("tex_displacement", 0);
//
//
//    points_shader_ = Shader("../shaders/PointsPolygon.vert",
//                            "../shaders/PointsPolygon.frag");
//    points_shader_.Bind();
//    points_shader_.SetUniform("tex_displacement", 0);
  }

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};

  Shader shader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
