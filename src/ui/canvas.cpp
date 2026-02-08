#include "lmgl/ui/canvas.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace lmgl {

namespace ui {

Canvas::Canvas(int width, int height) : m_width(width), m_height(height) { update_projection(); }

void Canvas::resize(int width, int height) {
    m_width = width;
    m_height = height;
    update_projection();
}

void Canvas::add_element(std::shared_ptr<UIElement> element) {
    if (!element)
        return;
    m_elements.push_back(element);
}

void Canvas::remove_element(std::shared_ptr<UIElement> element) {
    if (!element)
        return;
    auto it = std::find(m_elements.begin(), m_elements.end(), element);
    if (it != m_elements.end()) {
        m_elements.erase(it);
    }
}

void Canvas::render() {
    if (!m_visible)
        return;
    std::vector<std::shared_ptr<UIElement>> render_items;
    collect_render_items(render_items);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    for (const auto &item : render_items) {
        item->render(static_cast<float>(m_width), static_cast<float>(m_height));
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Canvas::update_projection() {
    m_projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
}

void Canvas::collect_render_items(std::vector<std::shared_ptr<UIElement>> &out_items) {
    out_items.clear();
    for (auto &element : m_elements) {
        collect_element_recursive(element, out_items);
    }
    std::sort(out_items.begin(), out_items.end(),
              [](const std::shared_ptr<UIElement> &a, const std::shared_ptr<UIElement> &b) {
                  return a->get_render_order() < b->get_render_order();
              });
}

void Canvas::collect_element_recursive(const std::shared_ptr<UIElement> element,
                                       std::vector<std::shared_ptr<UIElement>> &out_items) {
    if (!element->is_visible())
        return;
    out_items.push_back(element);
    for (const auto &child : element->get_children()) {
        collect_element_recursive(child, out_items);
    }
}

} // namespace ui

} // namespace lmgl
