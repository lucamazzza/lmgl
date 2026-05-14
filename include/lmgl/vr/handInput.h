/*!
 * @file handInput.h
 * @brief Utility helpers to extract simplified hand state from Leap data.
 */
#pragma once

#ifdef LMGL_LEAP

#include "lmgl/vr/leap.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/*!
 * @brief Compact hand representation derived from a Leap hand sample.
 */
struct HandState {
    //! @brief True when this hand sample is valid.
    bool      isTracked       = false;

    //! @brief True for left hand, false for right hand.
    bool      isLeft          = false;

    //! @brief Leap pinch strength in [0,1].
    float     pinchStrength   = 0.0f;

    //! @brief Leap grab strength in [0,1].
    float     grabStrength    = 0.0f;

    //! @brief Palm position in Leap tracking space.
    glm::vec3 palmPosition    = {};

    //! @brief Palm orientation in Leap tracking space.
    glm::quat palmOrientation = glm::quat(1, 0, 0, 0);

    //! @brief Index fingertip position.
    glm::vec3 indexTip        = {};

    //! @brief Thumb fingertip position.
    glm::vec3 thumbTip        = {};
};

/*!
 * @brief Converts a Leap hand into a compact HandState struct.
 *
 * @param lh Leap hand sample.
 * @return Extracted hand state.
 */
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

#endif
