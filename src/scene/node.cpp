#include "lmgl/scene/node.hpp"
#include "glm/fwd.hpp"

#include <glm/gtc/quaternion.hpp>

#include <algorithm>

namespace lmgl {

namespace scene {

Node::Node(const std::string& name) 
    : m_name(name) {
    update_local_transform();
}

// Transforms

void Node::set_position(const glm::vec3& position) {
    m_position = position;
    update_local_transform();
}

void Node::set_rotation(const glm::quat& rotation) {
    m_rotation = rotation;
    update_local_transform();
}

void Node::set_rotation(const glm::vec3& euler_angles) {
    glm::vec3 euler_rad = glm::radians(euler_angles);
    m_rotation = glm::quat(euler_rad);
    update_local_transform();
}

void Node::set_scale(const glm::vec3& scale) {
    m_scale = scale;
    update_local_transform();
}

void Node::set_scale(float scale) {
    m_scale = glm::vec3(scale);
    update_local_transform();
}

void Node::rotate(float angle, const glm::vec3& axis) {
    glm::quat delta_rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    m_rotation = delta_rotation * m_rotation;
    update_local_transform();
}

glm::vec3 Node::get_euler_angles() const {
    return glm::degrees(glm::eulerAngles(m_rotation));
}

void Node::look_at(const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 direction = glm::normalize(target - m_position);
    glm::quat look_rotation = glm::quatLookAt(direction, up);
    set_rotation(look_rotation);
}

// Hierarchy

void Node::add_child(std::shared_ptr<Node> child) {
    if (!child) return;
    child->detach_from_parent();
    child->m_parent = weak_from_this();
    m_children.push_back(child);
    child->update_transform(m_world_transform);
}

void Node::remove_child(std::shared_ptr<Node> child) {
    if (!child) return;
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent.reset();
        m_children.erase(it);
    }
}

void Node::detach_from_parent() {
    if (auto parent = m_parent.lock()) {
        parent->remove_child(shared_from_this());
    }
}

void Node::update_local_transform() {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotation = glm::mat4_cast(m_rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);
    m_local_transform = translation * rotation * scale;
}

void Node::update_transform(const glm::mat4& parent_transform) {
    m_world_transform = parent_transform * m_local_transform;
    for (auto& child : m_children) {
        child->update_transform(m_world_transform);
    }
}

} // namespace scene

} // namespace lmgl
