#include "Scene.h"

void Scene::Mesh::Render() const noexcept {
  // glBindVertexArray(vao);
  // glDrawElements(mode, indexCount, indexType,
  // reinterpret_cast<void*>(static_cast<std::uintptr_t>(indexOffset)));
}

void Scene::BindTextures() const noexcept {
  materials[0].albedo.BindSampler(1);
  //  glActiveTexture(GL_TEXTURE1);
  //  material.emission.Bind();
  //  glActiveTexture(GL_TEXTURE2);
  //  material.metal_rough.Bind();
  //  glActiveTexture(GL_TEXTURE3);
  //  material.normal.Bind();
  //  glActiveTexture(GL_TEXTURE4);
  //  material.occlusion.Bind();
}

void Scene::RenderModelNodes() const {
  throw std::runtime_error("RenderModelNodes called");
  // const tinygltf::Scene& scene = model.scenes[model.defaultScene];
  // for (size_t i = 0; i < scene.nodes.size(); ++i) {
  //   RenderModelNode(model.nodes[scene.nodes[i]]);
  // }
}

void Scene::RenderModelNode(const tinygltf::Node& node) const {
  // if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
  //   RenderMesh(model.meshes[node.mesh]);
  // }
  // for (size_t i = 0; i < node.children.size(); i++) {
  //   RenderModelNode(model.nodes[node.children[i]]);
  // }
}

void Scene::RenderMesh(const tinygltf::Mesh& mesh) const {
  // for (const auto& prim : primitives) {
  //   prim.Render();
  //   return;
  // }
}

void Scene::RenderPrimitive(int id) const {
  // primitives[id].Render();
}

// void Scene::Render(Type type, glm::mat4 transform) {
//   model_data_->BindTextures();
//   glUniformMatrix4fv(0, 1, false, glm::value_ptr(transform));
//   auto id = NodeIdByModelType(type);
//   auto model = model_data_->meshes[id];
//   for (int i = model.primitives_offset; i < model.primitives_num; ++i) {
//     model_data_->RenderPrimitive(i);
//   }
// }
//
// void Scene::Render(int type_id, glm::mat4 transform) {
//   model_data_->BindTextures();
//   auto model = model_data_->meshes[type_id];
//   for (int j = 0; j < model.transforms.size(); ++j) {
//     transform = glm::scale(model.transforms[j], glm::vec3(0.4f));
//     glUniformMatrix4fv(0, 1, false, glm::value_ptr(transform));
//     for (int i = model.primitives_offset; i < model.primitives_offset + model.primitives_num; ++i) {
//       model_data_->RenderPrimitive(i);
//     }
//   }
// }
