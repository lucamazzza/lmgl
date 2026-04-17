#include "lmgl/vr/ovr_backend.hpp"

#include <glm/geometric.hpp>

#if defined(LMGL_VR_OVR)
#if __has_include(<ovr.h>)
#include <ovr.h>
#elif __has_include(<OVR_CAPI.h>)
#include <OVR_CAPI.h>
#endif
#endif

namespace lmgl {

namespace vr {

#if defined(LMGL_VR_OVR)
struct OvrBackend::RuntimeState {
    ovrSession session = nullptr;
    ovrGraphicsLuid luid = {};
};
#endif

bool OvrBackend::initialize() {
    if (m_initialized)
        return true;

#if defined(LMGL_VR_OVR)
    m_runtime = std::make_unique<RuntimeState>();
    ovrResult result = ovr_Initialize(nullptr);
    if (OVR_FAILURE(result)) {
        m_runtime.reset();
        return false;
    }

    result = ovr_Create(&m_runtime->session, &m_runtime->luid);
    if (OVR_FAILURE(result)) {
        ovr_Shutdown();
        m_runtime.reset();
        return false;
    }

    m_ipd = ovr_GetFloat(m_runtime->session, OVR_KEY_IPD, m_ipd);
    m_has_runtime = true;
#endif

    m_initialized = true;
    return true;
}

void OvrBackend::shutdown() {
    if (!m_initialized)
        return;

#if defined(LMGL_VR_OVR)
    if (m_runtime && m_runtime->session) {
        ovr_Destroy(m_runtime->session);
        m_runtime->session = nullptr;
    }
    if (m_has_runtime) {
        ovr_Shutdown();
    }
    m_runtime.reset();
#endif

    m_has_runtime = false;
    m_initialized = false;
}

bool OvrBackend::is_initialized() const { return m_initialized; }

bool OvrBackend::has_runtime() const { return m_has_runtime; }

float OvrBackend::get_ipd() const { return m_ipd; }

void OvrBackend::set_fallback_ipd(float ipd) {
    if (ipd > 0.0f) {
        m_ipd = ipd;
    }
}

StereoCameraPair OvrBackend::build_stereo_cameras(const scene::Camera &center_camera) {
#if defined(LMGL_VR_OVR)
    if (m_has_runtime && m_runtime && m_runtime->session) {
        m_ipd = ovr_GetFloat(m_runtime->session, OVR_KEY_IPD, m_ipd);
    }
#endif

    StereoCameraPair pair;
    pair.left = std::make_shared<scene::Camera>(center_camera.get_fov(), center_camera.get_aspect(),
                                                center_camera.get_near_clip(), center_camera.get_far_clip());
    pair.right = std::make_shared<scene::Camera>(center_camera.get_fov(), center_camera.get_aspect(),
                                                 center_camera.get_near_clip(), center_camera.get_far_clip());

    const glm::vec3 center_pos = center_camera.get_position();
    const glm::vec3 center_target = center_camera.get_target();
    const glm::vec3 center_up = center_camera.get_up();

    glm::vec3 forward = center_target - center_pos;
    if (glm::length(forward) < 1e-6f) {
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    } else {
        forward = glm::normalize(forward);
    }

    glm::vec3 right_axis = glm::cross(forward, center_up);
    if (glm::length(right_axis) < 1e-6f) {
        right_axis = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
        right_axis = glm::normalize(right_axis);
    }

    const float half_ipd = m_ipd * 0.5f;
    const glm::vec3 eye_offset = right_axis * half_ipd;

    pair.left->set_position(center_pos - eye_offset);
    pair.left->set_target(center_target - eye_offset);
    pair.left->set_up(center_up);

    pair.right->set_position(center_pos + eye_offset);
    pair.right->set_target(center_target + eye_offset);
    pair.right->set_up(center_up);

    return pair;
}

} // namespace vr

} // namespace lmgl
