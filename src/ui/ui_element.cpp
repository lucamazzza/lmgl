#include "lmgl/ui/ui_element.hpp"

#include <algorithm>
#include <memory>

namespace lmgl {

namespace ui {

UIElement::UIElement(const std::string &name) : m_name(name) {}

void UIElement::add_child(std::shared_ptr<UIElement> child) {
    if (!child)
        return;
    child->m_parent = weak_from_this();
    m_children.push_back(child);
}

void UIElement::remove_child(std::shared_ptr<UIElement> child) {
    if (!child)
        return;
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent.reset();
        m_children.erase(it);
    }
}

glm::vec2 UIElement::get_absolute_position(float canvas_width, float canvas_height) const {
    glm::vec2 anchor_offset = get_anchor_offset(canvas_width, canvas_height);
    glm::vec2 absolute_pos = anchor_offset + m_position;
    auto parent = m_parent.lock();
    if (parent) {
        absolute_pos += parent->get_absolute_position(canvas_width, canvas_height);
    }
    return absolute_pos;
}

glm::vec2 UIElement::get_anchor_offset(float canvas_width, float canvas_height) const {
    switch (m_anchor) {
    case Anchor::TopLeft:
        return glm::vec2(0.0f, 0.0f);
    case Anchor::TopCenter:
        return glm::vec2(canvas_width * 0.5f, 0.0f);
    case Anchor::TopRight:
        return glm::vec2(canvas_width, 0.0f);
    case Anchor::CenterLeft:
        return glm::vec2(0.0f, canvas_height * 0.5f);
    case Anchor::Center:
        return glm::vec2(canvas_width * 0.5f, canvas_height * 0.5f);
    case Anchor::CenterRight:
        return glm::vec2(canvas_width, canvas_height * 0.5f);
    case Anchor::BottomLeft:
        return glm::vec2(0.0f, canvas_height);
    case Anchor::BottomCenter:
        return glm::vec2(canvas_width * 0.5f, canvas_height);
    case Anchor::BottomRight:
        return glm::vec2(canvas_width, canvas_height);
    case Anchor::Stretch:
        return glm::vec2(0.0f, 0.0f);
    default:
        return glm::vec2(0.0f, 0.0f);
    }
}

} // namespace ui

} // namespace lmgl
