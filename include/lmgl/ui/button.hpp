/*!
 * @file button.hpp
 * @brief Button UI element with callback support.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/ui/ui_element.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/text.hpp"

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace lmgl {

namespace ui {

/*!
 * @brief Button states for visual feedback.
 */
enum class ButtonState { Normal, Hovered, Pressed };

/*!
 * @brief Simple button UI element with text label.
 *
 * The Button class combines a Panel background with Text label,
 * providing visual feedback for hover and press states.
 */
class Button : public UIElement {
  public:
    /*!
     * @brief Constructor for Button.
     *
     * @param label Text to display on the button.
     * @param name Optional name for the element.
     */
    Button(const std::string &label = "Button", const std::string &name = "Button");

    /*!
     * @brief Destructor.
     */
    ~Button() override = default;

    /*!
     * @brief Set the button label text.
     *
     * @param label New label text.
     */
    void set_label(const std::string &label);

    /*!
     * @brief Get the button label text.
     *
     * @return Current label.
     */
    inline const std::string &get_label() const { return m_label; }

    /*!
     * @brief Set the click callback.
     *
     * @param callback Function to call when button is clicked.
     */
    inline void set_on_click(std::function<void()> callback) { m_on_click = callback; }

    /*!
     * @brief Set button colors for different states.
     *
     * @param normal Color when not interacted with.
     * @param hovered Color when mouse is over button.
     * @param pressed Color when button is pressed.
     */
    void set_colors(const glm::vec4 &normal, const glm::vec4 &hovered, const glm::vec4 &pressed);

    /*!
     * @brief Set text color.
     *
     * @param color Text color.
     */
    void set_text_color(const glm::vec4 &color);

    /*!
     * @brief Get the internal text element (for setting font).
     *
     * @return Pointer to text element.
     */
    inline std::shared_ptr<Text> get_text() { return m_text; }

    /*!
     * @brief Check if point is inside button.
     *
     * @param x X coordinate in screen space.
     * @param y Y coordinate in screen space.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if point is inside button.
     */
    bool contains_point(float x, float y, float canvas_width, float canvas_height) const;

    /*!
     * @brief Handle mouse button event.
     *
     * @param x Mouse X coordinate.
     * @param y Mouse Y coordinate.
     * @param pressed True if button pressed, false if released.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if event was handled.
     */
    bool handle_mouse_button(float x, float y, bool pressed, float canvas_width, float canvas_height);

    /*!
     * @brief Handle mouse movement event.
     *
     * @param x Mouse X coordinate.
     * @param y Mouse Y coordinate.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     */
    void handle_mouse_move(float x, float y, float canvas_width, float canvas_height);

    /*!
     * @brief Render the button.
     *
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @param projection Projection matrix.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Button label text
    std::string m_label;

    //! @brief Background panel
    std::shared_ptr<Panel> m_panel;

    //! @brief Text element
    std::shared_ptr<Text> m_text;

    //! @brief Click callback
    std::function<void()> m_on_click;

    //! @brief Current button state
    ButtonState m_state{ButtonState::Normal};

    //! @brief Colors for different states
    glm::vec4 m_color_normal{0.3f, 0.3f, 0.3f, 1.0f};
    glm::vec4 m_color_hovered{0.4f, 0.4f, 0.4f, 1.0f};
    glm::vec4 m_color_pressed{0.2f, 0.2f, 0.2f, 1.0f};

    //! @brief Text color
    glm::vec4 m_text_color{1.0f, 1.0f, 1.0f, 1.0f};
};

} // namespace ui

} // namespace lmgl
