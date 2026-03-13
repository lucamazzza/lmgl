/*!
 * @file input.hpp
 * @brief Static input wrapper for convenient input handling.
 *
 * Provides a simpler API for checking input without needing engine references.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/core/engine.hpp"
#include <glm/glm.hpp>

namespace lmgl {

/*!
 * @brief Static input helper class.
 *
 * Provides convenient static methods to check keyboard and mouse input
 * without needing to reference the Engine singleton everywhere.
 *
 * @note This is a convenience wrapper around Engine input methods.
 */
class Input {
  public:
    /*!
     * @brief Check if a key is currently pressed.
     *
     * @param key Key to check.
     * @return True if pressed.
     */
    static bool is_key_pressed(core::Key key);

    /*!
     * @brief Check if a key was just pressed this frame.
     *
     * @param key Key to check.
     * @return True if just pressed.
     */
    static bool is_key_just_pressed(core::Key key);

    /*!
     * @brief Check if a key was just released this frame.
     *
     * @param key Key to check.
     * @return True if just released.
     */
    static bool is_key_just_released(core::Key key);

    /*!
     * @brief Check if a mouse button is pressed.
     *
     * @param button Mouse button to check (0=left, 1=right, 2=middle).
     * @return True if pressed.
     */
    static bool is_mouse_button_pressed(int button);

    /*!
     * @brief Get mouse position.
     *
     * @return Mouse position as vec2(x, y).
     */
    static glm::vec2 get_mouse_position();

    /*!
     * @brief Get mouse position delta since last frame.
     *
     * @return Mouse delta as vec2(dx, dy).
     */
    static glm::vec2 get_mouse_delta();

    /*!
     * @brief Get scroll offset since last frame.
     *
     * @return Scroll offset as vec2(x_offset, y_offset).
     */
    static glm::vec2 get_scroll_offset();

    /*!
     * @brief Set mouse position.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     */
    static void set_mouse_position(double x, double y);

    /*!
     * @brief Set cursor mode.
     *
     * @param mode Cursor mode (Normal, Hidden, Disabled).
     */
    static void set_cursor_mode(core::CursorMode mode);
};

} // namespace lmgl
