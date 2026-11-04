#include "Door.h"

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

#include <iostream>

#include "Hinge.h"
#include "Scene.h"

void Door::Initialize(JPH::PhysicsSystem* physics_system,
  SceneNode* scene_node, JPH::BodyID frame_body_id,
  ma_engine* audio_engine, ma_sound_group* sfx_group) {
  if (physics_system_) {
    throw std::runtime_error("the door already initialized");
  }
  physics_system_ = physics_system;
  scene_node_ = scene_node;
  ma_sound_init_from_file(audio_engine, "../assets/audio/sound_coin.mp3", 0, sfx_group, NULL, &sound_creak_);
  ma_sound_set_looping(&sound_creak_, MA_TRUE);
  // ma_sound_set_spatialization_enabled(&sound_creak_, MA_FALSE); // dbg

  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  auto InitHinge = [&](Hinge* hinge) {
    JPH::HingeConstraintSettings hinge_settings;
    JPH::Vec3 pivot = hinge->GetSceneNode()->global_transform.GetTranslation();
    hinge_settings.mPoint1 = hinge_settings.mPoint2 = pivot;
    hinge_settings.mHingeAxis1 = hinge_settings.mHingeAxis2 = JPH::Vec3::sAxisY();
    JPH::Ref<JPH::Constraint> constraint = bi.CreateConstraint(
        &hinge_settings,
        frame_body_id,
        scene_node_->body_id
    );
    physics_system_->AddConstraint(constraint);
    hinge->SetConstraint(constraint);
  };
  for (Hinge* h : hinges_) {
    InitHinge(h);
  }
}

GameObject::InteractPrompt Door::GetInteractPrompt(Character* interactor) {
  bool all_broken = true;
  for (Hinge* h : hinges_) {
    if (!h->IsBroken()) {
      all_broken = false;
      break;
    }
  }
  if (all_broken) {
    return {};
  }
  return {"E to open the door", GetPosition()};
}

void Door::Interact(Character* interactor) {
  // SlowlyOpenByCharacter();
}

JPH::Vec3 Door::GetPosition() const {
  return scene_node_->global_bounds.GetCenter();
}

void Door::Update(JPH::BodyInterface* bi, float deltaTime) {
  JPH::Vec3 angVel = bi->GetAngularVelocity(scene_node_->body_id);
  float speed = angVel.Length();
  if (speed > 0.05f) {
    if (!ma_sound_is_playing(&sound_creak_)) {
      ma_sound_start(&sound_creak_);
    }
    float dynamicPitch = 0.6f + (speed * 0.3f);
    float dynamicVolume = std::clamp(speed * 0.5f, 0.1f, 1.0f);
    ma_sound_set_pitch(&sound_creak_, dynamicPitch);
    // ma_sound_set_volume(&sound_creak_, dynamicVolume); // TODO; too silent sound
    JPH::RVec3 pos = GetPosition();
    ma_sound_set_position(&sound_creak_, pos.GetX(), pos.GetY(), pos.GetZ());
  }
  else {
    if (ma_sound_is_playing(&sound_creak_)) {
      ma_sound_stop(&sound_creak_);
    }
  }
}
