// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/ContactListener.h>

// Tests the contact listener callbacks
class ContactListenerImpl : public JPH::ContactListener {
 public:
  // See: ContactListener
  virtual JPH::ValidateResult OnContactValidate(
      const JPH::Body &inBody1, const JPH::Body &inBody2,
      JPH::RVec3Arg inBaseOffset,
      const JPH::CollideShapeResult &inCollisionResult) override;
  virtual void OnContactAdded(const JPH::Body &inBody1,
                              const JPH::Body &inBody2,
                              const JPH::ContactManifold &inManifold,
                              JPH::ContactSettings &ioSettings) override;
  virtual void OnContactPersisted(const JPH::Body &inBody1,
                                  const JPH::Body &inBody2,
                                  const JPH::ContactManifold &inManifold,
                                  JPH::ContactSettings &ioSettings) override;
  virtual void OnContactRemoved(
      const JPH::SubShapeIDPair &inSubShapePair) override;

  void SetNextListener(JPH::ContactListener *inListener) { mNext = inListener; }

 private:
  // Map that keeps track of the current state of contacts based on the contact
  // listener callbacks
  using StatePair = std::pair<JPH::RVec3, JPH::ContactPoints>;
  using StateMap = JPH::UnorderedMap<JPH::SubShapeIDPair, StatePair>;
  JPH::Mutex mStateMutex;
  StateMap mState;

  JPH::ContactListener *mNext = nullptr;
};
