#ifndef WIREBOUNDWORLDCREATOR_ASSETPROPERTIES_H
#define WIREBOUNDWORLDCREATOR_ASSETPROPERTIES_H

#include <string>

struct AssetProperties {
  enum class CollisionType {
    Cube,
    Sphere,
    Capsule,
    Cylinder,
    Container,
    Cabinet,
  };

  enum class Type {
    Static,
    Dynamic,
    PointLight,
    PointLightFake,
    None,
    Hinge,
    HingeBase,
    HingeMoving,
    Portal,
    Zone,
    Tile,
    Terrain,
    Wall,
    Character,
    Player,
  };

  Type type = Type::Static;
  CollisionType collision_type = CollisionType::Cube;
  float mass = 10.0f;
  bool isPickable = false;
  bool isInteractable = false;
};

const AssetProperties& GetAssetDefaultProperties(const std::string& meshName);

#endif  // WIREBOUNDWORLDCREATOR_ASSETPROPERTIES_H
