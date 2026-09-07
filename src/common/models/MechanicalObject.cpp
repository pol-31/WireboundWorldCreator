#include "MechanicalObject.h"

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

SceneNode* LookForHingesLevel(SceneNode* node) {
  // Scene::ModelNode* hinge_level = nullptr; // that's where we separate bodies
  // for (auto child : node->children) {
  //   if (scene.meshes[child->mesh_index].type == Scene::Type::Hinge) {
  //     hinge_level = child;
  //     break;
  //   }
  // }
  // if (!hinge_level) {
  //   for (auto child : node->children) {
  //     hinge_level = LookForHingesLevel(child);
  //     if (hinge_level) {
  //       break;
  //     }
  //   }
  // }
  // return hinge_level;
}

JPH::AABox CalculateSumAabox(SceneNode* node) {
  JPH::AABox aabox;
  //TODO: recursively add
  return aabox;
}

MechanicalObject::MechanicalObject(SceneNode* model) {
  // Scene::ModelNode* hinge_level = LookForHingesLevel(model);
  // if (!hinge_level) {
  //   throw std::runtime_error("model has no hinges");
  // }
  // for (auto child : hinge_level->children) {
  //   if (scene.meshes[child->mesh_index].type == Scene::Type::Hinge) {
  //     /// hinge bodies already created (together) at scene parsing
  //   } else {
  //     JPH::AABox aabox = CalculateSumAabox(child);
  //     //TODO: create dynamic body
  //   }
  // }
  // // Connect the parts with a hinge
  // JPH::HingeConstraintSettings hinge;
  // hinge.mPoint1 = hinge.mPoint2 = JPH::RVec3(data.position.x, data.position.y, data.position.z - 1.5f);
  // hinge.mHingeAxis1 = hinge.mHingeAxis2 = JPH::Vec3::sAxisY();
  // mPhysicsSystem->AddConstraint(mBodyInterface->CreateConstraint(&hinge, b1, collision_id));
  // //TODO: create constraint
  // //TODO: create body for moving_part_aabb
}



bool MechanicalObject::TakeDamage(JPH::BodyID hit_body, float damage) {}
