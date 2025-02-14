#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_PLAYERCHARACTER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_PLAYERCHARACTER_H_

class PlayerCharacter {
 public:
  explicit PlayerCharacter(float speed) : speed_(speed) {}

  [[nodiscard]] float GetSpeed() const {
    return speed_;
  }

  [[nodiscard]] const float& GetSpeedRef() const {
    return speed_;
  }

 private:
  float speed_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_PLAYERCHARACTER_H_
