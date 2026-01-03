/*!
 * @file camera.hpp
 * @brief Declaration of the Camera class for managing viewport and projection.
 *
 * This file contains the declaration of the Camera class, which handles
 * view and projection transformations for rendering 3D scenes.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace lmgl {

namespace scene {

/*!
 * @brief Represents a camera for viewing the 3D scene.
 *
 * The Camera class provides methods to configure perspective or orthographic
 * projection, compute view matrices, and perform screen-to-world unprojection
 * for tasks like mouse picking.
 */
class Camera {
public:

    /*!
     * @brief Projection mode for the camera.
     */
    enum class ProjectionMode {
        Perspective,
        Orthographic
    };

    /*!
     * @brief Constructor for the Camera class.
     *
     * Initializes the camera with default perspective settings.
     *
     * @param fov Field of view in degrees (default: 45.0).
     * @param aspect Aspect ratio (width/height) (default: 16.0/9.0).
     * @param near Near clipping plane (default: 0.1).
     * @param far Far clipping plane (default: 100.0).
     */
    Camera(float fov = 45.0f, float aspect = 16.0f / 9.0f, float near = 0.1f, float far = 100.0f);

    /*!
     * @brief Set the camera to perspective projection mode.
     *
     * Configures the camera to use perspective projection with the specified parameters.
     *
     * @param fov Field of view in degrees.
     * @param aspect Aspect ratio (width/height).
     * @param near Near clipping plane.
     * @param far Far clipping plane.
     */
    void set_perspective(float fov, float aspect, float near, float far);

    /*!
     * @brief Set the camera to orthographic projection mode.
     *
     * Configures the camera to use orthographic projection with the specified bounds.
     *
     * @param left Left boundary.
     * @param right Right boundary.
     * @param bottom Bottom boundary.
     * @param top Top boundary.
     * @param near Near clipping plane.
     * @param far Far clipping plane.
     */
    void set_orthographic(float left, float right, float bottom, float top, float near, float far);

    /*!
     * @brief Set the camera position.
     *
     * Updates the position of the camera in world space.
     *
     * @param position New position vector.
     */
    inline void set_position(const glm::vec3& position) { m_position = position; m_view_dirty = true; }

    /*!
     * @brief Set the camera target/look-at point.
     *
     * Updates the point the camera is looking at.
     *
     * @param target Target position vector.
     */
    inline void set_target(const glm::vec3& target) { m_target = target; m_view_dirty = true; }

    /*!
     * @brief Set the camera's up vector.
     *
     * Updates the up direction for the camera orientation.
     *
     * @param up Up direction vector (default: (0, 1, 0)).
     */
    inline void set_up(const glm::vec3& up) { m_up = up; m_view_dirty = true; }

    /*!
     * @brief Get the camera position.
     *
     * @return Current position of the camera.
     */
    inline const glm::vec3& get_position() const { return m_position; }

    /*!
     * @brief Get the camera target.
     *
     * @return Current target/look-at point.
     */
    inline const glm::vec3& get_target() const { return m_target; }

    /*!
     * @brief Get the camera up vector.
     *
     * @return Current up direction vector.
     */
    inline const glm::vec3& get_up() const { return m_up; }

    /*!
     * @brief Get the view matrix.
     *
     * Returns the view matrix computed using glm::lookAt.
     * The matrix is cached and only recomputed when necessary.
     *
     * @return View matrix.
     */
    const glm::mat4& get_view_matrix() const;

    /*!
     * @brief Get the projection matrix.
     *
     * Returns the current projection matrix (perspective or orthographic).
     *
     * @return Projection matrix.
     */
    inline const glm::mat4& get_projection_matrix() const { return m_projection; }

    /*!
     * @brief Get the combined view-projection matrix.
     *
     * Returns the product of projection and view matrices.
     *
     * @return View-projection matrix.
     */
    glm::mat4 get_view_projection_matrix() const;

    /*!
     * @brief Unproject screen coordinates to world space.
     *
     * Converts 2D screen coordinates to a 3D ray in world space,
     * useful for mouse picking and raycasting.
     *
     * @param screen_x Screen X coordinate.
     * @param screen_y Screen Y coordinate.
     * @param screen_width Width of the viewport.
     * @param screen_height Height of the viewport.
     * @return Ray direction in world space (normalized).
     */
    glm::vec3 unproject(float screen_x, float screen_y, float screen_width, float screen_height) const;

    /*!
     * @brief Get the current projection mode.
     *
     * @return Current projection mode (Perspective or Orthographic).
     */
    inline ProjectionMode get_projection_mode() const { return m_mode; }

private:

    //! @brief Camera position in world space.
    glm::vec3 m_position;

    //! @brief Target/look-at point.
    glm::vec3 m_target;

    //! @brief Up direction vector.
    glm::vec3 m_up;

    //! @brief View matrix (cached).
    mutable glm::mat4 m_view;

    //! @brief Projection matrix.
    glm::mat4 m_projection;

    //! @brief Current projection mode.
    ProjectionMode m_mode;

    //! @brief Flag indicating if view matrix needs recalculation.
    mutable bool m_view_dirty;

    //! @brief Field of view (for perspective).
    float m_fov;

    //! @brief Aspect ratio.
    float m_aspect;

    //! @brief Near clipping plane.
    float m_near;

    //! @brief Far clipping plane.
    float m_far;
};

} // namespace scene

} // namespace lmgl
