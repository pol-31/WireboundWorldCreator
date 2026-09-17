#include "Door.h"

#include <iostream>

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include "Scene.h"

// Door::Door(JPH::PhysicsSystem* physics_system, const Scene* scene, ScenePortal* portal) {
//   std::vector<SceneNode*> hinges;
//   for (auto child : portal->frame->children) {
//     if (scene->scene_data_.meshes[child->mesh_index].type == Scene::Type::Hinge) {
//       hinges.push_back(child);
//     }
//   }
//   if (hinges.size() != 2) {
//     throw std::runtime_error("wrong door hinges (num != 2)");
//   }
//   JPH::HingeConstraintSettings hinge;
//   hinge.mPoint1 = hinge.mPoint2 = hinges[0]->global_bounds.GetCenter();
//   hinge.mHingeAxis1 = hinge.mHingeAxis2 = JPH::Vec3::sAxisY();
//   auto* bi = &physics_system->GetBodyInterface();
//   physics_system->AddConstraint(bi->CreateConstraint(
//     &hinge, hinges[0]->body_id, hinges[1]->body_id));
// }

Door::Door(JPH::PhysicsSystem* physics_system,
      const Scene* scene, ScenePortal* portal)
    : physics_system_(physics_system), portal_(portal) {
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  std::vector<SceneNode*> hinge_nodes;
  for (auto* child : portal->frame->children) {
    if (scene->scene_data_.meshes[child->mesh_index].type == Scene::Type::Hinge) {
      hinge_nodes.push_back(child);
    }
  }
  if (hinge_nodes.size() != 2) {
    throw std::runtime_error("wrong door hinges (num != 2)");
  }

  // 2. Create a constraint for EACH hinge
  for (SceneNode* h_node : hinge_nodes) {
    JPH::HingeConstraintSettings hinge_settings;

    // Use the global translation of the hinge node as the pivot point
    JPH::Vec3 pivot = h_node->global_transform.GetTranslation();

    // mSpace defaults to WorldSpace, so Jolt will automatically calculate
    // the local offsets for both the frame and the desk based on this world point
    hinge_settings.mPoint1 = hinge_settings.mPoint2 = pivot;
    hinge_settings.mHingeAxis1 = hinge_settings.mHingeAxis2 = JPH::Vec3::sAxisY(); // Assuming Y is up

    // CRITICAL FIX: Connect the Frame to the Desk!
    JPH::Constraint* constraint = bi.CreateConstraint(
        &hinge_settings,
        portal->frame->body_id,
        portal->desk->body_id
    );

    physics_system_->AddConstraint(constraint);

    // Store it so we can break it later
    hinges_.push_back({ h_node->body_id, constraint, false });
    std::cout << h_node->body_id.GetIndex() << std::endl;
  }
}

bool Door::TakeDamage(JPH::BodyID hit_body, JPH::Vec3 impulse, float damage) {
  bool all_broken = true;
  bool hit_registered = false;

  for (auto& hinge : hinges_) {
    if (!hinge.is_broken && hinge.hitbox_id == hit_body) {
      // Bullet hit this hinge! Break the constraint.
      physics_system_->RemoveConstraint(hinge.constraint);
      std::cout << "removed constraint" << std::endl;
      JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
      bi.SetMotionType(
          hinge.hitbox_id,             // The BodyID of the hinge you shot
          JPH::EMotionType::Dynamic,   // Switch it to dynamic
          JPH::EActivation::Activate   // Wake it up immediately so gravity takes effect
      );
     bi.AddImpulse(hinge.hitbox_id, impulse);

      // Optional: Also destroy the hinge's physical body so it disappears
      // or swap its mesh to a "broken" variant.
      // physics_system_->GetBodyInterface().RemoveBody(hinge.hitbox_id);
      // hinge_node->is_visible = false;

      hinge.is_broken = true;
      hit_registered = true;
    }

    if (!hinge.is_broken) {
      all_broken = false;
    }
  }

  // If all hinges are broken, Jolt will naturally let the dynamic DoorDesk
  // detach and fall to the ground due to gravity.
  return all_broken;
}
