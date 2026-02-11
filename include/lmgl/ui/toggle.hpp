/*!
 * @file toggle.hpp
 * @brief Toggle (checkbox) UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/ui/ui_element.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/text.hpp"

#include <glm/glm.hpp>
#include <functional>
#include <memory>
#include <string>

namespace lmgl {

namespace ui {

/*!
 * @brief Toggle/Checkbox UI element with on/off state.
 */
class Toggle : public UIElement {
  public:
    /*!
     * @brief Constructor for Toggle.
     *
     * @param label Text label for the toggle.
     * @param name Optional name for the element.
     */
    Toggle(const std::string &label = "Toggle", const std::string &name = "Toggle");

    /*!
     * @brief Destructor.
     */
    ~Toggle() override = default;

    /*!
     * @brief Set the toggle state.
     *
     * @param checked True for on, false for off.
     */
    void set_checked(bool checked);

    /*!
     * @brief Get the toggle state.
     *
     * @return True if checked, false otherwise.
     */
    inline bool is_checked() const { return m_checked; }

    /*!
     * @brief Set the label text.
     *
     * @param label New label.
     */
    void set_label(const std::string &label);

    /*!
     * @brief Get the internal text element (for setting font).
     *
     * @return Pointer to text element.
     */
    inline std::shared_ptr<Text> get_text() { return m_text; }

    /*!
     * @brief Set the callback for state changes.
     *
     * @param callback Function called with new state.
     */
    inline void set_on_toggle(std::function<void(bool)> callback) { m_on_toggle = callback; }

    /*!
     * @brief Set checkbox size (default 20x20).
     *
     * @param size Size of the checkbox.
     */
    inline void set_box_size(float size) { m_box_size = size; }

    /*!
     * @brief Check if point is inside toggle.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if inside.
     */
    bool contains_point(float x, float y, float canvas_width, float canvas_height) const;

    /*!
     * @brief Handle mouse click.
     *
     * @param x Mouse X.
     * @param y Mouse Y.
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @return True if handled.
     */
    bool handle_click(float x, float y, float canvas_width, float canvas_height);

    /*!
     * @brief Render the toggle.
     *
     * @param canvas_width Canvas width.
     * @param canvas_height Canvas height.
     * @param projection Projection matrix.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Label text
    std::string m_label;

    //! @brief Checked state
    bool m_checked{false};

    //! @brief Checkbox size
    float m_box_size{20.0f};

    //! @brief Box panel (background)
    std::shared_ptr<Panel> m_box_bg;

    //! @brief Checkmark panel
    std::shared_ptr<Panel> m_checkmark;

    //! @brief Label text
    std::shared_ptr<Text> m_text;

    //! @brief Toggle callback
    std::function<void(bool)> m_on_toggle;

    //! @brief Background colors
    glm::vec4 m_bg_color{0.2f, 0.2f, 0.2f, 1.0f};
    glm::vec4 m_check_color{0.0f, 1.0f, 0.0f, 1.0f};
    glm::vec4 m_text_color{1.0f, 1.0f, 1.0f, 1.0f};
};

} // namespace ui

} // namespace lmgl
