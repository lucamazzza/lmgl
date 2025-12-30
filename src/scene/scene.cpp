#include "lmgl/scene/scene.hpp"

namespace lmgl {

namespace scene {

Scene::Scene(const std::string& name) 
    : m_name(name), m_root(std::make_shared<Node>("Root")) {}

void Scene::update() {
    m_root->update_transform(glm::mat4(1.0f));
}

} // namespace scene

} // namespace lmgl
