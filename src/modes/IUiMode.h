#ifndef WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_

#include "UiSharedResources.h"

#include "../core/Ui.h"

class IUiMode {
 public:
  IUiMode(UiSharedResources& ui_shared_resources,
          UiStaticSprite&& sprite_mode)
      : ui_shared_resources_(ui_shared_resources),
        sprite_mode_(std::move(sprite_mode)) {}

  virtual void Render() = 0;
  virtual void RenderPicking()= 0;
  virtual data::TextId Hover(std::uint32_t global_id) = 0;

#ifndef NDEBUG
  virtual glm::uvec2 GetPrerenderTextIds() const noexcept {
    return {0, 0};
  }
#else
  virtual glm::uvec2 GetPrerenderTextIds() const noexcept = 0;
#endif

  // glfw callbacks, e.g. mouse scroll, mouse & keyboard btn interaction
  virtual void BindCallbacks() = 0;

  virtual std::vector<std::string> Serialize() {
    /*std::ofstream file(path.data());
    if (!file.is_open()) {
      std::cerr << "cannot open the file " << path << std::endl;
      return;
    }
    std::ostringstream all_data;
    for (const auto& graph : graphs) {
      all_data << std::to_string(graph.type_id) << ';'
               << std::to_string(graph.graph.vertices.size()) << ';'
               << std::to_string(graph.graph.vertices.size()) << ";\n";
      for (const auto& vertex : graph.graph.vertices) {
        all_data << std::to_string(vertex.x) << ';'
                 << std::to_string(vertex.y) << ";\n";
      }
      for (const auto& edge : graph.graph.edges) {
        all_data << std::to_string(edge.first) << ';'
                 << std::to_string(edge.second) << ";\n";
      }
    }*/
    return {};
  }

  //TODO: parse from where?
  virtual void Parse() {
    /*std::vector<GraphTraits> graphs{};
    if (path.empty()) {
      return graphs;
    }
    std::ifstream file(path.data());
    if (!file.is_open()) {
      std::cerr << "cannot open the file " << path << std::endl;
      return graphs;
    }
    std::string line;
    std::istringstream iss_line;
    while (std::getline(file, line)) {
      iss_line.clear();
      iss_line.str(line);
      // new graph instance
      GraphTraits graph;
      std::getline(iss_line, line, ';');
      graph.type_id = std::stoi(line);

      std::getline(iss_line, line, ';');
      int vertices_num = std::stoi(line);
      std::getline(iss_line, line, ';');
      int edges_num = std::stoi(line);

      for (int i = 0; i < vertices_num; ++i) {
        std::getline(file, line); // example: 10;20;
        size_t delimiterPos = line.find(';');
        graph.graph.vertices.push_back({
            std::stoi(line.substr(0, delimiterPos)),
            std::stoi(line.substr(delimiterPos + 1))
        });
      }
      for (int i = 0; i < edges_num; ++i) {
        std::getline(file, line); // example: 0;1;
        size_t delimiterPos = line.find(';');
        graph.graph.edges.push_back({
            std::stoi(line.substr(0, delimiterPos)),
            std::stoi(line.substr(delimiterPos + 1))
        });
      }
      graphs.push_back(graph);
    }
    return graphs;*/
  }

 protected:
  UiSharedResources& ui_shared_resources_;

  UiStaticSprite sprite_mode_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
