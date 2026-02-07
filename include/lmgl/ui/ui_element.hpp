/*!
 * @file ui_element.hpp
 * @brief Base class for all UI elements.
 *
 * This header file contains the definition of the UIElement class, which serves
 * as the base class for all UI components in the UI system. It provides basic
 * properties like position, size, visibility, and render order.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

namespace lmgl {

/*!
 * The namespace `ui` contains all classes and functions related to the user interface system.
 * This includes UI elements, layout managers, event handling, and rendering logic.
 * The UI system is designed to be flexible and extensible, allowing for the creation of complex user interfaces
 * with ease. The UI system supports features such as hierarchical element structures, event propagation, and
 * customizable rendering.
 */
namespace ui {

/*!
 * @brief Enumeration for UI element anchoring.
 *
 * This enumeration defines the various anchoring options for UI elements, which determine how they are
 * positioned and sized relative to their parent container. The anchoring options include:
 * - TopLeft: Anchors the element to the top-left corner of the parent container.
 * - TopCenter: Anchors the element to the top-center of the parent container.
 * - TopRight: Anchors the element to the top-right corner of the parent container.
 * - CenterLeft: Anchors the element to the center-left of the parent container.
 * - Center: Anchors the element to the center of the parent container.
 * - CenterRight: Anchors the element to the center-right of the parent container.
 * - BottomLeft: Anchors the element to the bottom-left corner of the parent container.
 * - BottomCenter: Anchors the element to the bottom-center of the parent container.
 * - BottomRight: Anchors the element to the bottom-right corner of the parent container.
 * - Stretch: Stretches the element to fill its parent container, ignoring any specified size.
 */
enum class Anchor {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Stretch
};
/*!
 * @brief Base class for all UI elements.
 *
 * The UIElement class provides common functionality for UI components,
 * including position, size, anchoring, visibility, and hierarchical
 * parent-child relationships.
 */
class UIElement : public std::enable_shared_from_this<UIElement> {
  public:
    /*!
     * @brief Constructor for UIElement.
     *
     * @param name Optional name for the UI element.
     */
    UIElement(const std::string &name = "UIElement");

    /*!
     * @brief Virtual destructor.
     */
    virtual ~UIElement() = default;

    /*!
     * @brief Set the position of the UI element.
     *
     * @param position Position in pixels (x, y).
     */
    inline void set_position(const glm::vec2 &position) { m_position = position; }

    /*!
     * @brief Set the size of the UI element.
     *
     * @param size Size in pixels (width, height).
     */
    inline void set_size(const glm::vec2 &size) { m_size = size; }

    /*!
     * @brief Set the anchor of the UI element.
     *
     * @param anchor Anchor preset.
     */
    inline void set_anchor(Anchor anchor) { m_anchor = anchor; }

    /*!
     * @brief Set the render order (z-index).
     *
     * Higher values render on top of lower values.
     *
     * @param order Render order value.
     */
    inline void set_render_order(int order) { m_render_order = order; }

    /*!
     * @brief Set visibility of the UI element.
     *
     * @param visible True to show, false to hide.
     */
    inline void set_visible(bool visible) { m_visible = visible; }

    /*!
     * @brief Set the name of the UI element.
     *
     * @param name New name for the element.
     */
    inline void set_name(const std::string &name) { m_name = name; }

    /*!
     * @brief Get the position of the UI element.
     *
     * @return Position in pixels.
     */
    inline const glm::vec2 &get_position() const { return m_position; }

    /*!
     * @brief Get the size of the UI element.
     *
     * @return Size in pixels.
     */
    inline const glm::vec2 &get_size() const { return m_size; }

    /*!
     * @brief Get the anchor of the UI element.
     *
     * @return Anchor preset.
     */
    inline Anchor get_anchor() const { return m_anchor; }

    /*!
     * @brief Get the render order.
     *
     * @return Render order value.
     */
    inline int get_render_order() const { return m_render_order; }

    /*!
     * @brief Check if the UI element is visible.
     *
     * @return True if visible, false otherwise.
     */
    inline bool is_visible() const { return m_visible; }

    /*!
     * @brief Get the name of the UI element.
     *
     * @return Name of the element.
     */
    inline const std::string &get_name() const { return m_name; }

    /*!
     * @brief Get the parent UI element.
     *
     * @return Shared pointer to the parent element, or nullptr if none.
     */
    inline std::shared_ptr<UIElement> get_parent() const { return m_parent.lock(); }

    /*!
     * @brief Add a child UI element.
     *
     * @param child Shared pointer to the child element.
     */
    void add_child(const std::shared_ptr<UIElement> &child);

    /*!
     * @brief Remove a child UI element.
     *
     * @param child Shared pointer to the child element.
     */
    void remove_child(const std::shared_ptr<UIElement> &child);

    /*!
     * @brief Get all child UI elements.
     *
     * @return Vector of shared pointers to child elements.
     */
    std::vector<std::shared_ptr<UIElement>> get_children() const { return m_children; }

    /*!
     * @brief Compute the absolute screen position based on anchor and parent.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     * @return Absolute position in pixels.
     */
    glm::vec2 get_absolute_position(float canvas_width, float canvas_height) const;

    /*!
     * @brief Virtual render method to be overridden by derived classes.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     */
    virtual void render(float canvas_width, float canvas_height) = 0;

  protected:
    //! @brief Name of the UI element
    std::string m_name;

    //! @brief Position in pixels (relative to anchor)
    glm::vec2 m_position{0.0f, 0.0f};

    //! @brief Size in pixels
    glm::vec2 m_size{100.0f, 100.0f};

    //! @brief Anchor preset
    Anchor m_anchor{Anchor::TopLeft};

    //! @brief Render order (z-index)
    int m_render_order{0};

    //! @brief Visibility flag
    bool m_visible{true};

    //! @brief Parent UI element
    std::weak_ptr<UIElement> m_parent;
    //! @brief Child UI elements
    std::vector<std::shared_ptr<UIElement>> m_children;

  private:
    /*!
     * @brief Get anchor offset based on canvas dimensions.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     * @return Anchor offset in pixels.
     */
    glm::vec2 get_anchor_offset(float canvas_width, float canvas_height) const;
};

} // namespace ui

} // namespace lmgl
