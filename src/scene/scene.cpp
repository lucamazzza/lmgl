#include "lmgl/scene/scene.hpp"
#include <memory>

namespace lmgl {

namespace scene {

Scene::Scene(const std::string& name) 
    : m_name(name), m_root(std::make_shared<Node>("Root")) {}

void Scene::update() {
    m_root->update_transform(glm::mat4(1.0f));
}

void Scene::add_light(std::shared_ptr<Light> light) {
    if (light) m_lights.push_back(light);
}

void Scene::remove_light(std::shared_ptr<Light> light) {
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end()) {
        m_lights.erase(it);
    }
}

void Scene::clear_lights() {
    m_lights.clear();
}

} // namespace scene

} // namespace lmgl
