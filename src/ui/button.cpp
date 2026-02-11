/*!
 * @file button.cpp
 * @brief Implementation of Button UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/button.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/text.hpp"

namespace lmgl {

namespace ui {

Button::Button(const std::string &label, const std::string &name) : UIElement(name), m_label(label) {
    m_panel = std::make_shared<Panel>("ButtonPanel");
    m_panel->set_color(m_color_normal);
    m_text = std::make_shared<Text>(label, "ButtonText");
    m_text->set_color(m_text_color);
    m_text->set_alignment(TextAlign::Center);
}

void Button::set_label(const std::string &label) {
    m_label = label;
    if (m_text) {
        m_text->set_text(label);
    }
}

void Button::set_colors(const glm::vec4 &normal, const glm::vec4 &hovered, const glm::vec4 &pressed) {
    m_color_normal = normal;
    m_color_hovered = hovered;
    m_color_pressed = pressed;
}

void Button::set_text_color(const glm::vec4 &color) {
    m_text_color = color;
    if (m_text) {
        m_text->set_color(color);
    }
}

bool Button::contains_point(float x, float y, float canvas_width, float canvas_height) const {
    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    return x >= abs_pos.x && x <= abs_pos.x + m_size.x && y >= abs_pos.y && y <= abs_pos.y + m_size.y;
}

bool Button::handle_mouse_button(float x, float y, bool pressed, float canvas_width, float canvas_height) {
    if (!m_visible)
        return false;

    bool inside = contains_point(x, y, canvas_width, canvas_height);
    if (pressed && inside) {
        m_state = ButtonState::Pressed;
        return true;
    } else if (!pressed && m_state == ButtonState::Pressed && inside) {
        m_state = ButtonState::Hovered;
        if (m_on_click) {
            m_on_click();
        }
        return true;
    } else if (!pressed) {
        m_state = inside ? ButtonState::Hovered : ButtonState::Normal;
    }
    return false;
}

void Button::handle_mouse_move(float x, float y, float canvas_width, float canvas_height) {
    if (!m_visible)
        return;

    bool inside = contains_point(x, y, canvas_width, canvas_height);
    if (m_state != ButtonState::Pressed) {
        m_state = inside ? ButtonState::Hovered : ButtonState::Normal;
    }
}

void Button::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible)
        return;

    switch (m_state) {
    case ButtonState::Normal:
        m_panel->set_color(m_color_normal);
        break;
    case ButtonState::Hovered:
        m_panel->set_color(m_color_hovered);
        break;
    case ButtonState::Pressed:
        m_panel->set_color(m_color_pressed);
        break;
    }
    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    m_panel->set_position(abs_pos);
    m_panel->set_size(m_size);
    m_panel->set_anchor(Anchor::TopLeft);
    m_text->set_position(glm::vec2(abs_pos.x + m_size.x * 0.5f, abs_pos.y + m_size.y * 0.5f - 16.0f));
    m_text->set_anchor(Anchor::TopLeft);
    m_panel->render(canvas_width, canvas_height, projection);
    m_text->render(canvas_width, canvas_height, projection);
}

} // namespace ui

} // namespace lmgl
