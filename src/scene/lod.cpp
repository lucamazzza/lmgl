#include "lmgl/scene/lod.hpp"
#include <memory>

namespace lmgl {

namespace scene {

void LOD::add_level(std::shared_ptr<Mesh> mesh, float max_distance) {
    if (!mesh)
        return;
    m_levels.emplace_back(mesh, max_distance);
}

std::shared_ptr<Mesh> LOD::get_mesh(float distance_sq) const {
    if (m_levels.empty())
        return nullptr;
    for (const auto &level : m_levels) {
        if (distance_sq <= level.max_distance_sq) {
            return level.mesh;
        }
    }
    return m_levels.back().mesh;
}

std::shared_ptr<Mesh> LOD::get_mesh(const glm::vec3 &camera_pos, const glm::vec3 &object_pos) const {
    glm::vec3 diff = camera_pos - object_pos;
    float distance_sq = glm::dot(diff, diff);
    return get_mesh(distance_sq);
}

} // namespace scene

} // namespace lmgl
