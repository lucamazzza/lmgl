/*!
 * @file slider.cpp
 * @brief Implementation of Slider UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/slider.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/text.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace lmgl {

namespace ui {

Slider::Slider(float min, float max, float initial_value, const std::string &name)
    : UIElement(name), m_min(min), m_max(max) {
    m_track_bg = std::make_shared<Panel>("SliderTrackBG");
    m_track_bg->set_color(m_track_color);
    m_track_fill = std::make_shared<Panel>("SliderTrackFill");
    m_track_fill->set_color(m_fill_color);
    m_handle = std::make_shared<Panel>("SliderHandle");
    m_handle->set_color(m_handle_color);
    m_label_text = std::make_shared<Text>("", "SliderLabel");
    m_label_text->set_color(m_text_color);
    m_value_text = std::make_shared<Text>("", "SliderValue");
    m_value_text->set_color(m_text_color);
    set_value(initial_value);
}

void Slider::set_value(float value) {
    float new_value = std::clamp(value, m_min, m_max);
    if (m_value != new_value) {
        m_value = new_value;
        if (m_show_value) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << m_value;
            m_value_text->set_text(ss.str());
        }
        if (m_on_value_changed) {
            m_on_value_changed(m_value);
        }
    }
}

void Slider::set_range(float min, float max) {
    m_min = min;
    m_max = max;
    set_value(m_value);
}

void Slider::set_label(const std::string &label) {
    m_label = label;
    m_label_text->set_text(label);
}

bool Slider::contains_point(float x, float y, float canvas_width, float canvas_height) const {
    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    float expanded_height = std::max(m_track_height, m_handle_size) + 4.0f;
    return x >= abs_pos.x && x <= abs_pos.x + m_size.x && y >= abs_pos.y - 2.0f && y <= abs_pos.y + expanded_height;
}

bool Slider::handle_mouse_button(float x, float y, bool pressed, float canvas_width, float canvas_height) {
    if (!m_visible)
        return false;

    if (pressed && contains_point(x, y, canvas_width, canvas_height)) {
        m_dragging = true;
        update_value_from_mouse(x, canvas_width, canvas_height);
        return true;
    } else if (!pressed) {
        m_dragging = false;
    }
    return false;
}

void Slider::handle_mouse_drag(float x, float y, float canvas_width, float canvas_height) {
    if (m_dragging) {
        update_value_from_mouse(x, canvas_width, canvas_height);
    }
}

void Slider::update_value_from_mouse(float mouse_x, float canvas_width, float canvas_height) {
    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    float track_width = m_size.x;
    float relative_x = mouse_x - abs_pos.x;
    float t = std::clamp(relative_x / track_width, 0.0f, 1.0f);
    float new_value = m_min + t * (m_max - m_min);
    set_value(new_value);
}

void Slider::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible)
        return;

    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);
    if (!m_label.empty() && m_label_text) {
        m_label_text->set_position(glm::vec2(abs_pos.x, abs_pos.y - 20.0f));
        m_label_text->set_anchor(Anchor::TopLeft);
        m_label_text->render(canvas_width, canvas_height, projection);
    }
    float track_y = abs_pos.y + (m_handle_size - m_track_height) * 0.5f;
    float t = (m_value - m_min) / (m_max - m_min);
    float handle_x = abs_pos.x + t * m_size.x - m_handle_size * 0.5f;
    float handle_y = abs_pos.y;
    m_track_bg->set_position(glm::vec2(abs_pos.x, track_y));
    m_track_bg->set_size(glm::vec2(m_size.x, m_track_height));
    m_track_bg->set_anchor(Anchor::TopLeft);
    m_track_bg->render(canvas_width, canvas_height, projection);
    if (t > 0.0f) {
        m_track_fill->set_position(glm::vec2(abs_pos.x, track_y));
        m_track_fill->set_size(glm::vec2(t * m_size.x, m_track_height));
        m_track_fill->set_anchor(Anchor::TopLeft);
        m_track_fill->render(canvas_width, canvas_height, projection);
    }
    m_handle->set_position(glm::vec2(handle_x, handle_y));
    m_handle->set_size(glm::vec2(m_handle_size, m_handle_size));
    m_handle->set_anchor(Anchor::TopLeft);
    m_handle->render(canvas_width, canvas_height, projection);
    if (m_show_value && m_value_text) {
        m_value_text->set_position(glm::vec2(abs_pos.x + m_size.x + 10.0f, abs_pos.y - 8.0f));
        m_value_text->set_anchor(Anchor::TopLeft);
        m_value_text->render(canvas_width, canvas_height, projection);
    }
}

} // namespace ui

} // namespace lmgl
