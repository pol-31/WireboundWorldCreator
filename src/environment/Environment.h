#ifndef WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_ENVIRONMENT_H_
#define WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_ENVIRONMENT_H_

#include "../common/Paths.h"
#include "Time.h"
#include "Wind.h"

class Environment {
 public:
  explicit Environment(const Paths& paths);
  ~Environment() = default;

  [[nodiscard]] const Time& GetTime() const noexcept {
    return time_;
  }
  [[nodiscard]] const Wind& GetWind() const noexcept {
    return wind_;
  }

  void Update();

 private:
  /// initializing at ctor initializer list,
  /// so can pass ubo id to other members ctors
  class Ubo {
   public:
    Ubo();
    ~Ubo();

    [[nodiscard]] GLuint GetId() const {
      return id_;
    }

   private:
    void Init();
    void DeInit();

    GLuint id_{0};
  };

  Ubo ubo_;
  Time time_;
  Wind wind_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_ENVIRONMENT_H_
