#ifndef WIREBOUNDWORLDCREATOR_SHOOTDECALINFO_H
#define WIREBOUNDWORLDCREATOR_SHOOTDECALINFO_H

struct ShootDecalInfo {
  JPH::Mat44 transform = JPH::Mat44::sIdentity();
  int type = 0; // material, shot angle
};

#endif  // WIREBOUNDWORLDCREATOR_SHOOTDECALINFO_H
