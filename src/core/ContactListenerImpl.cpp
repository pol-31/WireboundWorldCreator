// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include "ContactListenerImpl.h"

// #include <Renderer/Renderer.h>

JPH::ValidateResult ContactListenerImpl::OnContactValidate(
    const JPH::Body &inBody1, const JPH::Body &inBody2,
    JPH::RVec3Arg inBaseOffset,
    const JPH::CollideShapeResult &inCollisionResult) {
  // Check ordering contract between body 1 and body 2
  bool contract = inBody1.GetMotionType() >= inBody2.GetMotionType() ||
                  (inBody1.GetMotionType() == inBody2.GetMotionType() &&
                   inBody1.GetID() < inBody2.GetID());
  if (!contract) JPH_BREAKPOINT;

  JPH::ValidateResult result;
  if (mNext != nullptr)
    result = mNext->OnContactValidate(inBody1, inBody2, inBaseOffset,
                                      inCollisionResult);
  else
    result = ContactListener::OnContactValidate(inBody1, inBody2, inBaseOffset,
                                                inCollisionResult);

  JPH::RVec3 contact_point = inBaseOffset + inCollisionResult.mContactPointOn1;
  // DebugRenderer::sInstance->DrawArrow(
  //     contact_point,
  //     contact_point -
  //         inCollisionResult.mPenetrationAxis.NormalizedOr(Vec3::sZero()),
  //     Color::sBlue, 0.05f);

  // JPH::Trace("Validate %u and %u result %d", inBody1.GetID().GetIndex(),
             // inBody2.GetID().GetIndex(), (int)result);

  return result;
}

void ContactListenerImpl::OnContactAdded(const JPH::Body &inBody1,
                                         const JPH::Body &inBody2,
                                         const JPH::ContactManifold &inManifold,
                                         JPH::ContactSettings &ioSettings) {
  // Expect bodies to be sorted
  if (!(inBody1.GetID() < inBody2.GetID())) JPH_BREAKPOINT;

  // JPH::Trace("Contact added %u (%08x) and %u (%08x)",
  //            inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(),
  //            inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

  // DebugRenderer::sInstance->DrawWirePolygon(
  //     RMat44::sTranslation(inManifold.mBaseOffset),
  //     inManifold.mRelativeContactPointsOn1, Color::sGreen, 0.05f);
  // DebugRenderer::sInstance->DrawWirePolygon(
  //     RMat44::sTranslation(inManifold.mBaseOffset),
  //     inManifold.mRelativeContactPointsOn2, Color::sGreen, 0.05f);
  // DebugRenderer::sInstance->DrawArrow(
  //     inManifold.GetWorldSpaceContactPointOn1(0),
  //     inManifold.GetWorldSpaceContactPointOn1(0) +
  //     inManifold.mWorldSpaceNormal, Color::sGreen, 0.05f);

  // Insert new manifold into state map
  {
    std::lock_guard lock(mStateMutex);
    JPH::SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1,
                            inBody2.GetID(), inManifold.mSubShapeID2);
    if (mState.find(key) != mState.end())
      JPH_BREAKPOINT;  // Added contact that already existed
    mState[key] =
        StatePair(inManifold.mBaseOffset, inManifold.mRelativeContactPointsOn1);
  }

  if (mNext != nullptr)
    mNext->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void ContactListenerImpl::OnContactPersisted(
    const JPH::Body &inBody1, const JPH::Body &inBody2,
    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) {
  // Expect bodies to be sorted
  if (!(inBody1.GetID() < inBody2.GetID())) JPH_BREAKPOINT;

  //TODO: this is actually good
  // JPH::Trace("Contact persisted %u (%08x) and %u (%08x)",
  //            inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(),
  //            inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

  // DebugRenderer::sInstance->DrawWirePolygon(
  //     RMat44::sTranslation(inManifold.mBaseOffset),
  //     inManifold.mRelativeContactPointsOn1, Color::sYellow, 0.05f);
  // DebugRenderer::sInstance->DrawWirePolygon(
  //     RMat44::sTranslation(inManifold.mBaseOffset),
  //     inManifold.mRelativeContactPointsOn2, Color::sYellow, 0.05f);
  // DebugRenderer::sInstance->DrawArrow(
  //     inManifold.GetWorldSpaceContactPointOn1(0),
  //     inManifold.GetWorldSpaceContactPointOn1(0) +
  //     inManifold.mWorldSpaceNormal, Color::sYellow, 0.05f);

  // Update existing manifold in state map
  {
    std::lock_guard lock(mStateMutex);
    JPH::SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1,
                            inBody2.GetID(), inManifold.mSubShapeID2);
    StateMap::iterator i = mState.find(key);
    if (i != mState.end())
      i->second = StatePair(inManifold.mBaseOffset,
                            inManifold.mRelativeContactPointsOn1);
    else
      JPH_BREAKPOINT;  // Persisted contact that didn't exist
  }

  if (mNext != nullptr)
    mNext->OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
}

void ContactListenerImpl::OnContactRemoved(
    const JPH::SubShapeIDPair &inSubShapePair) {
  // Expect bodies to be sorted
  if (!(inSubShapePair.GetBody1ID() < inSubShapePair.GetBody2ID()))
    JPH_BREAKPOINT;

  // JPH::Trace("Contact removed %u (%08x) and %u (%08x)",
  //            inSubShapePair.GetBody1ID().GetIndex(),
  //            inSubShapePair.GetSubShapeID1().GetValue(),
  //            inSubShapePair.GetBody2ID().GetIndex(),
  //            inSubShapePair.GetSubShapeID2().GetValue());

  // Update existing manifold in state map
  {
    std::lock_guard lock(mStateMutex);
    StateMap::iterator i = mState.find(inSubShapePair);
    if (i != mState.end())
      mState.erase(i);
    else
      JPH_BREAKPOINT;  // Removed contact that didn't exist
  }

  if (mNext != nullptr) mNext->OnContactRemoved(inSubShapePair);
}
