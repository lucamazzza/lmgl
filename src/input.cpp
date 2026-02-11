/*!
 * @file input.cpp
 * @brief Implementation of static input wrapper.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/input.hpp"

namespace lmgl {

bool Input::is_key_pressed(core::Key key) { return core::Engine::get_instance().is_key_pressed(key); }

bool Input::is_key_just_pressed(core::Key key) { return core::Engine::get_instance().is_key_just_pressed(key); }

bool Input::is_key_just_released(core::Key key) { return core::Engine::get_instance().is_key_just_released(key); }

bool Input::is_mouse_button_pressed(int button) {
    return core::Engine::get_instance().is_mouse_button_pressed(static_cast<core::MouseButton>(button));
}

glm::vec2 Input::get_mouse_position() {
    auto &engine = core::Engine::get_instance();
    return glm::vec2(engine.get_mouse_x(), engine.get_mouse_y());
}

glm::vec2 Input::get_mouse_delta() {
    auto &engine = core::Engine::get_instance();
    double dx, dy;
    engine.get_mouse_delta(dx, dy);
    return glm::vec2(dx, dy);
}

glm::vec2 Input::get_scroll_offset() {
    auto &engine = core::Engine::get_instance();
    double x, y;
    engine.get_scroll_offset(x, y);
    return glm::vec2(x, y);
}

void Input::set_mouse_position(double x, double y) { core::Engine::get_instance().set_mouse_position(x, y); }

void Input::set_cursor_mode(core::CursorMode mode) { core::Engine::get_instance().set_cursor_mode(mode); }

} // namespace lmgl
