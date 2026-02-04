#include "lmgl/scene/light.hpp"

namespace lmgl {

namespace scene {

Light::Light(LightType type) : m_type(type) {}

std::shared_ptr<Light> Light::create_directional(const glm::vec3 &direction, const glm::vec3 &color) {
    auto light = std::make_shared<Light>(LightType::Directional);
    light->set_direction(direction);
    light->set_color(color);
    return light;
}

std::shared_ptr<Light> Light::create_point(const glm::vec3 &position, float range, const glm::vec3 &color) {
    auto light = std::make_shared<Light>(LightType::Point);
    light->set_position(position);
    light->set_range(range);
    light->set_color(color);
    return light;
}

std::shared_ptr<Light> Light::create_spot(const glm::vec3 &position, const glm::vec3 &direction, float angle,
                                          const glm::vec3 &color) {
    auto light = std::make_shared<Light>(LightType::Spot);
    light->set_position(position);
    light->set_direction(direction);
    light->set_outer_cone(glm::radians(angle));
    light->set_inner_cone(glm::radians(angle * 0.8f));
    light->set_color(color);
    return light;
}

} // namespace scene

} // namespace lmgl
