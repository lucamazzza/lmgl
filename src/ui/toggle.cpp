/*!
 * @file toggle.cpp
 * @brief Implementation of Toggle UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/toggle.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/text.hpp"

namespace lmgl {

namespace ui {

Toggle::Toggle(const std::string &label, const std::string &name) : UIElement(name), m_label(label) {
    m_box_bg = std::make_shared<Panel>("ToggleBox");
    m_box_bg->set_color(m_bg_color);
    m_checkmark = std::make_shared<Panel>("Checkmark");
    m_checkmark->set_color(m_check_color);
    m_checkmark->set_visible(false);
    m_text = std::make_shared<Text>(label, "ToggleLabel");
    m_text->set_color(m_text_color);
}

void Toggle::set_checked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        m_checkmark->set_visible(checked);
        if (m_on_toggle) {
            m_on_toggle(checked);
        }
    }
}

void Toggle::set_label(const std::string &label) {
    m_label = label;
    if (m_text) {
        m_text->set_text(label);
    }
}

bool Toggle::contains_point(float x, float y, float canvas_width, float canvas_height) const {
    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    return x >= abs_pos.x && x <= abs_pos.x + m_size.x && y >= abs_pos.y && y <= abs_pos.y + m_box_size;
}

bool Toggle::handle_click(float x, float y, float canvas_width, float canvas_height) {
    if (!m_visible)
        return false;

    if (contains_point(x, y, canvas_width, canvas_height)) {
        set_checked(!m_checked);
        return true;
    }

    return false;
}

void Toggle::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible)
        return;

    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    m_box_bg->set_position(abs_pos);
    m_box_bg->set_size(glm::vec2(m_box_size, m_box_size));
    m_box_bg->set_anchor(Anchor::TopLeft);
    m_box_bg->render(canvas_width, canvas_height, projection);
    if (m_checked) {
        float margin = m_box_size * 0.25f;
        m_checkmark->set_position(glm::vec2(abs_pos.x + margin, abs_pos.y + margin));
        m_checkmark->set_size(glm::vec2(m_box_size - margin * 2.0f, m_box_size - margin * 2.0f));
        m_checkmark->set_anchor(Anchor::TopLeft);
        m_checkmark->render(canvas_width, canvas_height, projection);
    }
    m_text->set_position(glm::vec2(abs_pos.x + m_box_size + 8.0f, abs_pos.y - 5.0f));
    m_text->set_anchor(Anchor::TopLeft);
    m_text->render(canvas_width, canvas_height, projection);
}

} // namespace ui

} // namespace lmgl
