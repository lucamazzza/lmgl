#include "lmgl/scene/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace lmgl {

namespace scene {

Camera::Camera(float fov, float aspect, float near, float far)
    : m_position(0.0f, 0.0f, 3.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_view(1.0f)
    , m_projection(1.0f)
    , m_mode(ProjectionMode::Perspective)
    , m_view_dirty(true)
    , m_fov(fov)
    , m_aspect(aspect)
    , m_near(near)
    , m_far(far) {
    set_perspective(fov, aspect, near, far);
}

void Camera::set_perspective(float fov, float aspect, float near, float far) {
    m_fov = fov;
    m_aspect = aspect;
    m_near = near;
    m_far = far;
    m_mode = ProjectionMode::Perspective;
    m_projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::set_orthographic(float left, float right, float bottom, float top, float near, float far) {
    m_near = near;
    m_far = far;
    m_mode = ProjectionMode::Orthographic;
    m_projection = glm::ortho(left, right, bottom, top, near, far);
}

const glm::mat4 &Camera::get_view_matrix() const {
    if (m_view_dirty) {
        m_view = glm::lookAt(m_position, m_target, m_up);
        m_view_dirty = false;
    }
    return m_view;
}

glm::mat4 Camera::get_view_projection_matrix() const { return m_projection * get_view_matrix(); }

glm::vec3 Camera::unproject(float screen_x, float screen_y, float screen_width, float screen_height) const {
    float x = (2.0f * screen_x) / screen_width - 1.0f;
    float y = 1.0f - (2.0f * screen_y) / screen_height;
    glm::vec4 ray_clip(x, y, -1.0f, 1.0f);
    glm::mat4 inv_projection = glm::inverse(m_projection);
    glm::vec4 ray_eye = inv_projection * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    glm::mat4 inv_view = glm::inverse(get_view_matrix());
    glm::vec3 ray_world = glm::vec3(inv_view * ray_eye);
    return glm::normalize(ray_world);
}

} // namespace scene

} // namespace lmgl
