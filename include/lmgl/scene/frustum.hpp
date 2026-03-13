/*!
 * @file frustum.hpp
 * @brief Defines frustum and bounding volume structures for 3D scene management.
 *
 * This file includes definitions for Axis-Aligned Bounding Boxes (AABB),
 * Bounding Spheres, Planes, and Frustums, along with their associated methods
 * for transformations and containment checks.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */
#pragma once

#include <array>
#include <glm/glm.hpp>

namespace lmgl {

namespace scene {

/*!
 * @brief Axis-Aligned Bounding Box (AABB) structure.
 *
 * Represents a 3D bounding box defined by minimum and maximum corner points.
 * Provides methods for transformations, expansion, and merging with other AABBs.
 */
struct AABB {
    //! @brief Minimum corner point of the AABB.
    glm::vec3 min;

    //! @brief Maximum corner point of the AABB.
    glm::vec3 max;

    /*!
     * @brief Default constructor initializing an empty AABB.
     *
     * Initializes min and max to zero vectors.
     */
    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

    /*!
     * @brief Parameterized constructor.
     *
     * Initializes the AABB with specified minimum and maximum corner points.
     *
     * @param min Minimum corner point.
     * @param max Maximum corner point.
     */
    AABB(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max) {}

    /*!
     * @brief Get the center point of the AABB.
     *
     * Calculates and returns the center point of the AABB.
     *
     * @return Center point as a glm::vec3.
     */
    inline glm::vec3 get_center() const { return (min + max) * 0.5f; }

    /*!
     * @brief Get the extents of the AABB.
     *
     * Calculates and returns the extents (half-size) of the AABB.
     *
     * @return Extents as a glm::vec3.
     */
    inline glm::vec3 get_extents() const { return (max - min) * 0.5f; }

    /*!
     * @brief Transform the AABB with a matrix.
     *
     * Applies a transformation matrix to the AABB and returns a new transformed AABB.
     *
     * @param matrix Transformation matrix.
     * @return Transformed AABB.
     */
    AABB transform(const glm::mat4 &matrix) const;

    /*!
     * @brief Expand the AABB to include a point.
     *
     * Expands the AABB to ensure it contains the specified point.
     *
     * @param point Point to include in the AABB.
     */
    void expand(const glm::vec3 &point);

    /*!
     * @brief Merge the AABB with another AABB.
     *
     * Expands the current AABB to include the volume of another AABB.
     *
     * @param other The other AABB to merge with.
     */
    void merge(const AABB &other);
};

/*!
 * @brief Bounding Sphere structure.
 *
 * Represents a sphere defined by a center point and a radius.
 * Provides methods for creating a bounding sphere from an AABB
 * and for transforming the sphere with a matrix.
 */
struct BoundingSphere {
    //! @brief Center point of the bounding sphere.
    glm::vec3 center;

    //! @brief Radius of the bounding sphere.
    float radius;

    /*!
     * @brief Default constructor initializing an empty bounding sphere.
     *
     * Initializes center to zero vector and radius to zero.
     */
    BoundingSphere() : center(glm::vec3(0.0f)), radius(0.0f) {}

    /*!
     * @brief Parameterized constructor.
     *
     * Initializes the bounding sphere with specified center and radius.
     *
     * @param center Center point of the sphere.
     * @param radius Radius of the sphere.
     */
    BoundingSphere(const glm::vec3 &center, float radius) : center(center), radius(radius) {}

    /*!
     * @brief Create a bounding sphere from an AABB.
     *
     * Constructs a bounding sphere that encompasses the given AABB.
     *
     * @param aabb The AABB to create the bounding sphere from.
     * @return BoundingSphere that contains the AABB.
     */
    static BoundingSphere from_aabb(const AABB &aabb);

    /*!
     * @brief Transform the bounding sphere with a matrix.
     *
     * Applies a transformation matrix to the bounding sphere and returns a new transformed sphere.
     *
     * @param matrix Transformation matrix.
     * @return Transformed BoundingSphere.
     */
    BoundingSphere transform(const glm::mat4 &matrix) const;
};

/*!
 * @brief Plane structure.
 *
 * Represents a plane in 3D space defined by a normal vector and a distance
 * from the origin. Provides methods for calculating the distance from a point
 * to the plane and for normalizing the plane equation.
 */
struct Plane {
    //! @brief Normal vector of the plane.
    glm::vec3 normal;

    //! @brief Distance from the origin to the plane along the normal.
    float distance;

    /*!
     * @brief Default constructor initializing a horizontal plane at the origin.
     *
     * Initializes the normal to (0, 1, 0) and distance to 0.
     */
    Plane() : normal(glm::vec3(0.0f, 1.0f, 0.0f)), distance(0.0f) {}

    /*!
     * @brief Parameterized constructor.
     *
     * Initializes the plane with specified normal and distance.
     *
     * @param normal Normal vector of the plane.
     * @param distance Distance from the origin to the plane.
     */
    Plane(const glm::vec3 &normal, float distance) : normal(normal), distance(distance) {}

    /*!
     * @brief Construct a plane from a point and a normal vector.
     *
     * Initializes the plane such that it passes through the given point
     * with the specified normal vector.
     *
     * @param point A point on the plane.
     * @param normal Normal vector of the plane.
     */
    Plane(const glm::vec3 &point, const glm::vec3 &normal)
        : normal(glm::normalize(normal)), distance(glm::dot(this->normal, point)) {}

    /*!
     * @brief Calculate the distance from a point to the plane.
     *
     * Computes the signed distance from the specified point to the plane.
     *
     * @param point The point to measure the distance to.
     * @return Signed distance from the point to the plane.
     */
    inline float distance_to_point(const glm::vec3 &point) const { return glm::dot(normal, point) - distance; }

    /*!
     * @brief Normalize the plane equation.
     *
     * Normalizes the plane's normal vector and adjusts the distance accordingly.
     */
    void normalize();
};

/*!
 * @brief Frustum structure.
 *
 * Represents a view frustum defined by six planes (left, right, bottom, top, near, far).
 * Provides methods for updating the frustum from a view-projection matrix and
 * for checking containment of points, spheres, and AABBs.
 */
class Frustum {
  public:
    /*!
     * @brief Enumeration for the frustum planes.
     *
     * Defines indices for accessing the six planes of the frustum.
     * - Left: Left plane
     * - Right: Right plane
     * - Bottom: Bottom plane
     * - Top: Top plane
     * - Near: Near plane
     * - Far: Far plane
     */
    enum PlaneIndex { Left = 0, Right, Bottom, Top, Near, Far };

    /*!
     * @brief Update the frustum planes from a view-projection matrix.
     *
     * Extracts the six frustum planes from the provided view-projection matrix.
     *
     * @param view_projection The combined view-projection matrix.
     */
    void update(const glm::mat4 &view_projection);

    /*!
     * @brief Check if a point is inside the frustum.
     *
     * Determines whether the specified point is contained within the frustum.
     *
     * @param point The point to check.
     * @return True if the point is inside the frustum, false otherwise.
     */
    bool contains_point(const glm::vec3 &point) const;

    /*!
     * @brief Check if a bounding sphere is inside the frustum.
     *
     * Determines whether the specified bounding sphere is contained within the frustum.
     *
     * @param sphere The bounding sphere to check.
     * @return True if the sphere is inside the frustum, false otherwise.
     */
    bool contains_sphere(const BoundingSphere &sphere) const;

    /*!
     * @brief Check if an Axis-Aligned Bounding Box (AABB) is inside the frustum.
     *
     * Determines whether the specified AABB is contained within the frustum.
     *
     * @param aabb The AABB to check.
     * @return True if the AABB is inside the frustum, false otherwise.
     */
    bool contains_aabb(const AABB &aabb) const;

    /*!
     * @brief Get a specific frustum plane.
     *
     * Retrieves the plane corresponding to the given index.
     *
     * @param index The index of the plane to retrieve.
     * @return Reference to the requested Plane.
     */
    inline const Plane &get_plane(PlaneIndex index) const { return m_planes[index]; }

  private:
    //! @brief Array of six planes defining the frustum.
    std::array<Plane, 6> m_planes;
};

} // namespace scene

} // namespace lmgl
