/*!
 * @file canvas.hpp
 * @brief Canvas for 2D UI rendering with orthographic projection.
 *
 * This header file contains the definition of the Canvas class, which provides
 * a 2D coordinate space for UI elements. It handles orthographic projection,
 * blending for transparency, and back-to-front rendering order.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/ui/ui_element.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace lmgl {

namespace ui {

/*!
 * @brief Canvas for rendering 2D UI elements.
 *
 * The Canvas class provides a 2D rendering surface that sits on top of the 3D world.
 * It manages orthographic projection, automatic window resizing, blending for
 * transparency, and proper render ordering of UI elements.
 */
class Canvas {
  public:
    /*!
     * @brief Constructor for Canvas.
     *
     * @param width Initial width of the canvas in pixels.
     * @param height Initial height of the canvas in pixels.
     */
    Canvas(int width = 1920, int height = 1080);

    /*!
     * @brief Destructor for Canvas.
     */
    ~Canvas() = default;

    /*!
     * @brief Resize the canvas to match window dimensions.
     *
     * Updates the orthographic projection matrix to match the new dimensions.
     *
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void resize(int width, int height);

    /*!
     * @brief Add a UI element to the canvas.
     *
     * @param element Shared pointer to the UI element.
     */
    void add_element(std::shared_ptr<UIElement> element);

    /*!
     * @brief Remove a UI element from the canvas.
     *
     * @param element Shared pointer to the UI element.
     */
    void remove_element(std::shared_ptr<UIElement> element);

    /*!
     * @brief Get all UI elements in the canvas.
     *
     * @return Vector of shared pointers to UI elements.
     */
    inline const std::vector<std::shared_ptr<UIElement>> &get_elements() const { return m_elements; }

    /*!
     * @brief Render all UI elements.
     *
     * Enables blending, sets up orthographic projection, and renders
     * all UI elements in back-to-front order based on their render order.
     */
    void render();

    /*!
     * @brief Get the orthographic projection matrix.
     *
     * Maps pixels (0, 0) to (width, height) to normalized device coordinates (-1, -1) to (1, 1).
     *
     * @return Orthographic projection matrix.
     */
    inline const glm::mat4 &get_projection_matrix() const { return m_projection; }

    /*!
     * @brief Get the canvas width.
     *
     * @return Width in pixels.
     */
    inline int get_width() const { return m_width; }

    /*!
     * @brief Get the canvas height.
     *
     * @return Height in pixels.
     */
    inline int get_height() const { return m_height; }

    /*!
     * @brief Set visibility of the entire canvas.
     *
     * @param visible True to show, false to hide.
     */
    inline void set_visible(bool visible) { m_visible = visible; }

    /*!
     * @brief Check if the canvas is visible.
     *
     * @return True if visible, false otherwise.
     */
    inline bool is_visible() const { return m_visible; }

  private:
    //! @brief Width of the canvas in pixels
    int m_width;

    //! @brief Height of the canvas in pixels
    int m_height;

    //! @brief Orthographic projection matrix
    glm::mat4 m_projection;

    //! @brief List of UI elements on the canvas
    std::vector<std::shared_ptr<UIElement>> m_elements;

    //! @brief Visibility flag
    bool m_visible = true;

    /*!
     * @brief Update the orthographic projection matrix.
     *
     * Creates a projection matrix that maps pixel coordinates to NDC.
     */
    void update_projection();

    /*!
     * @brief Collect all renderable elements (including children) and sort by render order.
     *
     * @param out_elements Vector to store the collected elements.
     */
    void collect_render_items(std::vector<std::shared_ptr<UIElement>> &out_items);

    /*!
     * @brief Recursively collect UI elements from hierarchy.
     *
     * @param element Current element to process.
     * @param out_elements Vector to store the collected elements.
     */
    void collect_element_recursive(const std::shared_ptr<UIElement> element,
                                   std::vector<std::shared_ptr<UIElement>> &out_items);
};

} // namespace ui

} // namespace lmgl
