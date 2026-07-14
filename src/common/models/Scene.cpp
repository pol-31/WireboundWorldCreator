#include "Scene.h"

#include <iostream>

#include <jolt/Physics/Body/BodyLock.h>

JPH::Mat44 Scene::NodePose::Matrix() const noexcept {
  return JPH::Mat44::sTranslation(t) *
         JPH::Mat44::sRotationTranslation(r, JPH::Vec3::sZero()) *
         JPH::Mat44::sScale(s);
}

/// by default, we're doing parent * local,
/// but in case attached jph body, we should take it instead,
/// because we created it on that first global pos,
/// so everything in the hands of JPH since then
void Scene::UpdateRenderTransform(const JPH::BodyLockInterface& bli,
                                  std::vector<Tile*>& tiles) {
  std::function<void(ModelNode*, const JPH::Mat44&)> dfs =
      [&](ModelNode* node, const JPH::Mat44& parent) {
        auto local = node->local_transform.Matrix();
        JPH::Mat44 global_transform;

        JPH::BodyID body_id = node->body_id;
        if (body_id.IsInvalid()) {
          global_transform = parent * local;
        } else {
          JPH::BodyLockRead lock(bli, body_id);
          if (lock.SucceededAndIsInBroadPhase()) {
            const JPH::Body& body = lock.GetBody();
            node->bounds = body.GetWorldSpaceBounds();
            global_transform = body.GetWorldTransform();
          } else {
            global_transform = parent * local;
          }
        }

        node->global_transform = global_transform;
        for (auto child : node->children) {
          dfs(child, global_transform);
        }
      };

  for (auto& tile : tiles) {
    for (auto node : tile->object_nodes) {
      dfs(node, JPH::Mat44::sIdentity());
    }
    for (auto& zone : tile->zones) {
      for (auto node : zone->object_nodes) {
        dfs(node, JPH::Mat44::sIdentity());
      }
    }
  }
}
