#include "lmgl/scene/frustum.hpp"

#include <algorithm>

namespace lmgl {

namespace scene {

// AABB

AABB AABB::transform(const glm::mat4 &matrix) const {
    glm::vec3 corners[8] = {{min.x, min.y, min.z}, {max.x, min.y, min.z}, {min.x, max.y, min.z}, {max.x, max.y, min.z},
                            {min.x, min.y, max.z}, {max.x, min.y, max.z}, {min.x, max.y, max.z}, {max.x, max.y, max.z}};
    AABB result;
    result.min = glm::vec3(std::numeric_limits<float>::max());
    result.max = glm::vec3(std::numeric_limits<float>::lowest());
    for (const auto &corner : corners) {
        glm::vec3 transformed = glm::vec3(matrix * glm::vec4(corner, 1.0f));
        result.min = glm::min(result.min, transformed);
        result.max = glm::max(result.max, transformed);
    }
    return result;
}

void AABB::expand(const glm::vec3 &point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABB::merge(const AABB &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

// BoundingSphere

BoundingSphere BoundingSphere::from_aabb(const AABB &aabb) {
    glm::vec3 center = aabb.get_center();
    float radius = glm::length(aabb.max - center);
    return BoundingSphere(center, radius);
}

BoundingSphere BoundingSphere::transform(const glm::mat4 &matrix) const {
    glm::vec3 transformed_center = glm::vec3(matrix * glm::vec4(center, 1.0f));
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(matrix[0]));
    scale.y = glm::length(glm::vec3(matrix[1]));
    scale.z = glm::length(glm::vec3(matrix[2]));
    float max_scale = glm::max(scale.x, glm::max(scale.y, scale.z));
    return BoundingSphere(transformed_center, radius * max_scale);
}

// Plane

void Plane::normalize() {
    float length = glm::length(normal);
    normal /= length;
    distance /= length;
}

// Frustum

void Frustum::update(const glm::mat4 &view_projection) {
    m_planes[Left].normal.x = view_projection[0][3] + view_projection[0][0];
    m_planes[Left].normal.y = view_projection[1][3] + view_projection[1][0];
    m_planes[Left].normal.z = view_projection[2][3] + view_projection[2][0];
    m_planes[Left].distance = view_projection[3][3] + view_projection[3][0];
    m_planes[Right].normal.x = view_projection[0][3] - view_projection[0][0];
    m_planes[Right].normal.y = view_projection[1][3] - view_projection[1][0];
    m_planes[Right].normal.z = view_projection[2][3] - view_projection[2][0];
    m_planes[Right].distance = view_projection[3][3] - view_projection[3][0];
    m_planes[Bottom].normal.x = view_projection[0][3] + view_projection[0][1];
    m_planes[Bottom].normal.y = view_projection[1][3] + view_projection[1][1];
    m_planes[Bottom].normal.z = view_projection[2][3] + view_projection[2][1];
    m_planes[Bottom].distance = view_projection[3][3] + view_projection[3][1];
    m_planes[Top].normal.x = view_projection[0][3] - view_projection[0][1];
    m_planes[Top].normal.y = view_projection[1][3] - view_projection[1][1];
    m_planes[Top].normal.z = view_projection[2][3] - view_projection[2][1];
    m_planes[Top].distance = view_projection[3][3] - view_projection[3][1];
    m_planes[Near].normal.x = view_projection[0][3] + view_projection[0][2];
    m_planes[Near].normal.y = view_projection[1][3] + view_projection[1][2];
    m_planes[Near].normal.z = view_projection[2][3] + view_projection[2][2];
    m_planes[Near].distance = view_projection[3][3] + view_projection[3][2];
    m_planes[Far].normal.x = view_projection[0][3] - view_projection[0][2];
    m_planes[Far].normal.y = view_projection[1][3] - view_projection[1][2];
    m_planes[Far].normal.z = view_projection[2][3] - view_projection[2][2];
    m_planes[Far].distance = view_projection[3][3] - view_projection[3][2];
    for (auto &plane : m_planes) {
        plane.normalize();
    }
}

bool Frustum::contains_point(const glm::vec3 &point) const {
    for (const auto &plane : m_planes) {
        if (plane.distance_to_point(point) < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Frustum::contains_sphere(const BoundingSphere &sphere) const {
    for (const auto &plane : m_planes) {
        float distance = plane.distance_to_point(sphere.center);
        if (distance < -sphere.radius) {
            return false;
        }
    }
    return true;
}

bool Frustum::contains_aabb(const AABB &aabb) const {
    for (const auto &plane : m_planes) {
        glm::vec3 positive_vertex = aabb.min;
        if (plane.normal.x >= 0)
            positive_vertex.x = aabb.max.x;
        if (plane.normal.y >= 0)
            positive_vertex.y = aabb.max.y;
        if (plane.normal.z >= 0)
            positive_vertex.z = aabb.max.z;
        if (plane.distance_to_point(positive_vertex) < 0.0f)
            return false;
    }
    return true;
}

} // namespace scene

} // namespace lmgl
