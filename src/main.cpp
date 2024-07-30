#include <iostream>

#include <fstream>
#include <sstream>

#include <stb_image.h>
#include <stb_image_write.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: we still need PLACEMENT (add models loading)

#include "SetupWindow.h"
#include "Details.h"
#include "Terrain.h"
#include "Cubemap.h"
#include "Cursor.h"

#include "GlobalGlfwCallbackData.h"

#include "UiComponents.h"
#include "UiCore.h"

void RunLoop(GLFWwindow* window) {
  Shader shader_static_sprite("../shaders/StaticSprite.vert",
                              "../shaders/StaticSprite.frag");

  Cursor cursor("/home/pavlo/Desktop/cursor_WireboundWorldCreator.png");

  shader_static_sprite.Bind();
  shader_static_sprite.SetUniform("tex", 0);

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(8 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(vao);

  std::vector<std::string_view> faces_sea {
      "../assets/sky/right.jpg", "../assets/sky/left.jpg",
      "../assets/sky/top.jpg", "../assets/sky/bottom.jpg",
      "../assets/sky/front.jpg", "../assets/sky/back.jpg"
  };
  std::vector<std::string_view> faces_hills {
      "../assets/sky/hills_ft.tga", "../assets/sky/hills_bk.tga",
      "../assets/sky/hills_up.tga", "../assets/sky/hills_dn.tga",
      "../assets/sky/hills_rt.tga", "../assets/sky/hills_lf.tga",
  };
  Cubemap cubemap(faces_hills, "../shaders/Skybox.vert", "../shaders/Skybox.frag");

  Terrain terrain(
      "../shaders/Terrain.vert", "../shaders/Terrain.frag",
      "../assets/terrain/map_terrain_height.png",
      "../assets/terrain/map_terrain_material.png",
      "../assets/terrain/map_terrain_occlusion.png",
      "../shaders/TerrainPicking.vert", "../shaders/TerrainPicking.frag"
  );

  glActiveTexture(GL_TEXTURE0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0, details::kTexCoordsSize,
      details::kTexPositionDefault.data());
  glBufferSubData(
      GL_ARRAY_BUFFER, details::kTexPositionSize,
      details::kTexCoordsSize, details::kTexCoordsDefault.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(vao);
  shader_static_sprite.Bind();

  Camera camera; // TODO: by now terminate() because of uninitialized glad/glfw
  GlobalGlfwCallbackData global_data(camera, cursor, terrain);
  glfwSetWindowUserPointer(window, reinterpret_cast<void*>(&global_data));

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.7f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    ProcessInput(window, camera);
    camera.UpdateViewMatrix();
    camera.UpdateProjectionMatrix();

    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    terrain.Render();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    cubemap.Render();
/*
    // cursor always positioned at the centre of the screen
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      terrain.SetScale(terrain.GetScale() * 2.0f);
      std::cout << "new scale " << terrain.GetScale() << std::endl;
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
      terrain.SetScale(terrain.GetScale() / 2.0f);
      std::cout << "new scale " << terrain.GetScale() << std::endl;
//      glfwGetCursorPos(window, &cur_pos_x, &cur_pos_y);
    }*/
    terrain.SetScale(world_scale);

    cursor.Render(shader_static_sprite, vao, vbo);

    glfwPollEvents(); // TODO: is there correct order
    glfwSwapBuffers(window);
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

int main() {
  auto window = SetupWindow();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  RunLoop(window);
  glfwTerminate();
  return 0;
}
