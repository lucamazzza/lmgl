#pragma once

#include "lmgl/vr/leap.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct HandState {
    bool      isTracked       = false;
    bool      isLeft          = false;
    float     pinchStrength   = 0.0f;
    float     grabStrength    = 0.0f;
    glm::vec3 palmPosition    = {};
    glm::quat palmOrientation = glm::quat(1, 0, 0, 0);
    glm::vec3 indexTip        = {};
    glm::vec3 thumbTip        = {};
};

inline HandState extractHandState(const LEAP_HAND& lh) {
    HandState hs;
    hs.isTracked       = true;
    hs.isLeft          = (lh.type == eLeapHandType_Left);
    hs.pinchStrength   = lh.pinch_strength;
    hs.grabStrength    = lh.grab_strength;
    hs.palmPosition    = glm::vec3(lh.palm.position.x,
                                   lh.palm.position.y,
                                   lh.palm.position.z);
    hs.palmOrientation = glm::quat(lh.palm.orientation.w,
                                   lh.palm.orientation.x,
                                   lh.palm.orientation.y,
                                   lh.palm.orientation.z);
    hs.indexTip        = glm::vec3(lh.digits[1].bones[3].next_joint.x,
                                   lh.digits[1].bones[3].next_joint.y,
                                   lh.digits[1].bones[3].next_joint.z);
    hs.thumbTip        = glm::vec3(lh.digits[0].bones[3].next_joint.x,
                                   lh.digits[0].bones[3].next_joint.y,
                                   lh.digits[0].bones[3].next_joint.z);
    return hs;
}