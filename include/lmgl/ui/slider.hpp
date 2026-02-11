/*!
 * @file slider.hpp
 * @brief Slider UI element for value selection.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/ui/ui_element.hpp"

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace lmgl {

namespace ui {

class Panel;
class Text;

/*!
 * @brief Horizontal slider UI element.
 */
class Slider : public UIElement {
  public:
    /*!
     * @brief Constructor for Slider.
     *
     * @param min Minimum value.
     * @param max Maximum value.
     * @param initial_value Initial value.
     * @param name Optional name.
     */
    Slider(float min = 0.0f, float max = 1.0f, float initial_value = 0.5f, const std::string &name = "Slider");

    /*!
     * @brief Destructor.
     */
    ~Slider() override = default;

    /*!
     * @brief Set the slider value.
     *
     * @param value New value (will be clamped to min/max).
     */
    void set_value(float value);

    /*!
     * @brief Get the current value.
     *
     * @return Current value.
     */
    inline float get_value() const { return m_value; }

    /*!
     * @brief Set the value range.
     *
     * @param min Minimum value.
     * @param max Maximum value.
     */
    void set_range(float min, float max);

    /*!
     * @brief Set the label text.
     *
     * @param label Label text.
     */
    void set_label(const std::string &label);

    /*!
     * @brief Set callback for value changes.
     *
     * @param callback Function called with new value.
     */
    inline void set_on_value_changed(std::function<void(float)> callback) { m_on_value_changed = callback; }

    /*!
     * @brief Set whether to show the value as text.
     *
     * @param show True to show value.
     */
    inline void set_show_value(bool show) { m_show_value = show; }

    /*!
     * @brief Get the label text element (for setting font).
     *
     * @return Pointer to label text.
     */
    inline std::shared_ptr<Text> get_label_text() { return m_label_text; }

    /*!
     * @brief Get the value text element (for setting font).
     *
     * @return Pointer to value text.
     */
    inline std::shared_ptr<Text> get_value_text() { return m_value_text; }

    /*!
     * @brief Check if point is inside slider.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if inside.
     */
    bool contains_point(float x, float y, float canvas_width, float canvas_height) const;

    /*!
     * @brief Handle mouse button event.
     *
     * @param x Mouse X.
     * @param y Mouse Y.
     * @param pressed True if pressed.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if handled.
     */
    bool handle_mouse_button(float x, float y, bool pressed, float canvas_width, float canvas_height);

    /*!
     * @brief Handle mouse drag.
     *
     * @param x Mouse X.
     * @param y Mouse Y.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     */
    void handle_mouse_drag(float x, float y, float canvas_width, float canvas_height);

    /*!
     * @brief Render the slider.
     *
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @param projection Projection matrix.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Current value
    float m_value;

    //! @brief Min/max range
    float m_min;
    float m_max;

    //! @brief Is being dragged
    bool m_dragging{false};

    //! @brief Label text
    std::string m_label;

    //! @brief Show value as text
    bool m_show_value{true};

    //! @brief Track height
    float m_track_height{4.0f};

    //! @brief Handle size
    float m_handle_size{16.0f};

    //! @brief Track background
    std::shared_ptr<Panel> m_track_bg;

    //! @brief Filled track
    std::shared_ptr<Panel> m_track_fill;

    //! @brief Slider handle
    std::shared_ptr<Panel> m_handle;

    //! @brief Label text
    std::shared_ptr<Text> m_label_text;

    //! @brief Value text
    std::shared_ptr<Text> m_value_text;

    //! @brief Value changed callback
    std::function<void(float)> m_on_value_changed;

    //! @brief Colors
    glm::vec4 m_track_color{0.3f, 0.3f, 0.3f, 1.0f};
    glm::vec4 m_fill_color{0.0f, 0.6f, 1.0f, 1.0f};
    glm::vec4 m_handle_color{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 m_text_color{1.0f, 1.0f, 1.0f, 1.0f};

    /*!
     * @brief Update value from mouse position.
     *
     * @param mouse_x Mouse X coordinate.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     */
    void update_value_from_mouse(float mouse_x, float canvas_width, float canvas_height);
};

} // namespace ui

} // namespace lmgl
