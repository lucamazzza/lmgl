/*!
 * @file panel.hpp
 * @brief Panel UI element for displaying colored rectangles.
 *
 * This header file contains the definition of the Panel class, which renders
 * a solid colored rectangle with optional border and background color.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/vertex_array.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace lmgl {

namespace ui {

/*!
 * @brief Panel UI element for rendering colored rectangles.
 *
 * The Panel class provides a basic colored rectangle UI element with
 * configurable background color and optional transparency.
 */
class Panel : public UIElement {
  public:
    /*!
     * @brief Constructor for Panel.
     *
     * @param name Optional name for the panel.
     */
    Panel(const std::string &name = "Panel");

    /*!
     * @brief Destructor for Panel.
     */
    ~Panel() override = default;

    /*!
     * @brief Set the background color of the panel.
     *
     * @param color RGBA color (values 0.0-1.0).
     */
    inline void set_color(const glm::vec4 &color) { m_color = color; }

    /*!
     * @brief Get the background color of the panel.
     *
     * @return RGBA color.
     */
    inline const glm::vec4 &get_color() const { return m_color; }

    /*!
     * @brief Render the panel.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     * @param projection Projection matrix for UI rendering.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Background color (RGBA)
    glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};

    //! @brief Shader for rendering the panel
    static std::shared_ptr<renderer::Shader> s_shader;

    //! @brief Vertex array for quad rendering
    static std::shared_ptr<renderer::VertexArray> s_vao;

    //! @brief Initialize static rendering resources
    static void initialize_resources();

    //! @brief Check if resources are initialized
    static bool s_initialized;
};

} // namespace ui

} // namespace lmgl
