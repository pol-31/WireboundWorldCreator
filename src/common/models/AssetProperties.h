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

  enum class AudioMaterial {
    Metal = 0,
    Wood = 1,
    Glass = 2,
    Coin = 3,
    Flesh = 4,
    // Stone = 2,
    // Dirt = 3,
    // Flesh = 4
};

  Type type = Type::Static;
  CollisionType collision_type = CollisionType::Cube;
  float mass = 10.0f;
  bool isPickable = false;
  bool isInteractable = false;
  AudioMaterial audio_material = AudioMaterial::Metal;
  std::string interaction_prompt = ""; // e.g., "Open", "Pick Up", "Inspect"
};

const AssetProperties& GetAssetDefaultProperties(const std::string& meshName);

#endif  // WIREBOUNDWORLDCREATOR_ASSETPROPERTIES_H
