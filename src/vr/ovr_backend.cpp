#include "lmgl/vr/ovr_backend.hpp"

#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>

#if defined(LMGL_VR_OVR)
#include <ovr.h>
#endif

namespace lmgl {

namespace vr {

#if defined(LMGL_VR_OVR)
struct OvrBackend::RuntimeState {
    std::unique_ptr<OvVR> helper;
};

static float extract_ipd_from_helper(const OvVR &helper, float fallback) {
    const glm::mat4 left_eye_to_head = helper.getEye2HeadMatrix(OvVR::EYE_LEFT);
    const glm::mat4 right_eye_to_head = helper.getEye2HeadMatrix(OvVR::EYE_RIGHT);
    const glm::mat4 left_head_to_eye = glm::inverse(left_eye_to_head);
    const glm::mat4 right_head_to_eye = glm::inverse(right_eye_to_head);
    const float ipd = glm::length(glm::vec3(right_head_to_eye[3]) - glm::vec3(left_head_to_eye[3]));
    if (ipd <= 0.0f)
        return fallback;
    return ipd;
}
#endif

bool OvrBackend::initialize() {
    if (m_initialized)
        return true;

#if defined(LMGL_VR_OVR)
    m_runtime = std::make_unique<RuntimeState>();
    m_runtime->helper = std::make_unique<OvVR>();
    if (!m_runtime->helper || !m_runtime->helper->init()) {
        m_runtime.reset();
        return false;
    }
    if (!m_runtime->helper->update()) {
        m_runtime->helper->free();
        m_runtime.reset();
        return false;
    }
    m_ipd = extract_ipd_from_helper(*m_runtime->helper, m_ipd);
    m_has_runtime = true;
#endif

    m_initialized = true;
    return true;
}

void OvrBackend::shutdown() {
    if (!m_initialized)
        return;

#if defined(LMGL_VR_OVR)
    if (m_runtime && m_runtime->helper) {
        m_runtime->helper->free();
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
    if (m_has_runtime && m_runtime && m_runtime->helper) {
        m_runtime->helper->update();
        m_ipd = extract_ipd_from_helper(*m_runtime->helper, m_ipd);
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

    glm::vec3 base_forward = center_target - center_pos;
    if (glm::length(base_forward) < 1e-6f) {
        base_forward = glm::vec3(0.0f, 0.0f, -1.0f);
    } else {
        base_forward = glm::normalize(base_forward);
    }

    glm::vec3 base_right = glm::cross(base_forward, center_up);
    if (glm::length(base_right) < 1e-6f) {
        base_right = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
        base_right = glm::normalize(base_right);
    }
    glm::vec3 base_up = glm::normalize(glm::cross(base_right, base_forward));
    glm::mat3 base_rotation(base_right, base_up, -base_forward);

    glm::vec3 head_local_position(0.0f);
    glm::mat3 head_local_rotation(1.0f);
    glm::vec3 left_eye_local(-0.5f * m_ipd, 0.0f, 0.0f);
    glm::vec3 right_eye_local(0.5f * m_ipd, 0.0f, 0.0f);

#if defined(LMGL_VR_OVR)
    if (m_has_runtime && m_runtime && m_runtime->helper) {
        const glm::mat4 hmd_mat = m_runtime->helper->getModelviewMatrix();
        head_local_position = glm::vec3(hmd_mat[3]);
        head_local_rotation = glm::mat3(hmd_mat);
        const glm::mat4 left_eye_to_head = m_runtime->helper->getEye2HeadMatrix(OvVR::EYE_LEFT);
        const glm::mat4 right_eye_to_head = m_runtime->helper->getEye2HeadMatrix(OvVR::EYE_RIGHT);
        left_eye_local = glm::vec3(glm::inverse(left_eye_to_head)[3]);
        right_eye_local = glm::vec3(glm::inverse(right_eye_to_head)[3]);
    }
#endif

    const glm::mat3 head_world_rotation = base_rotation * head_local_rotation;
    const glm::vec3 head_world_position = center_pos + base_rotation * head_local_position;

    const glm::vec3 left_pos = head_world_position + head_world_rotation * left_eye_local;
    const glm::vec3 right_pos = head_world_position + head_world_rotation * right_eye_local;
    const glm::vec3 world_forward = glm::normalize(head_world_rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::vec3 world_up = glm::normalize(head_world_rotation * glm::vec3(0.0f, 1.0f, 0.0f));

    pair.left->set_position(left_pos);
    pair.left->set_target(left_pos + world_forward);
    pair.left->set_up(world_up);

    pair.right->set_position(right_pos);
    pair.right->set_target(right_pos + world_forward);
    pair.right->set_up(world_up);

    return pair;
}

bool OvrBackend::pass(OvrEye eye, unsigned int texture_id) {
#if defined(LMGL_VR_OVR)
    if (!m_has_runtime || !m_runtime || !m_runtime->helper || texture_id == 0)
        return false;
    m_runtime->helper->pass(eye == OvrEye::Left ? OvVR::EYE_LEFT : OvVR::EYE_RIGHT, texture_id);
    return true;
#else
    (void)eye;
    (void)texture_id;
    return false;
#endif
}

void OvrBackend::render() {
#if defined(LMGL_VR_OVR)
    if (!m_has_runtime || !m_runtime || !m_runtime->helper)
        return;
    m_runtime->helper->render();
#endif
}

} // namespace vr

} // namespace lmgl
