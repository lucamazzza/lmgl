/*!
 * @file text.hpp
 * @brief Text UI element for rendering text strings.
 *
 * This header file contains the Text class which renders text using
 * a loaded font with support for color, alignment, and wrapping.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/vertex_array.hpp"
#include "lmgl/ui/font.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace lmgl {

namespace ui {

/*!
 * @brief Text alignment options.
 */
enum class TextAlign { Left, Center, Right };

/*!
 * @brief Text UI element for rendering strings.
 *
 * The Text class renders text using a Font, with support for color,
 * alignment, and automatic size calculation.
 */
class Text : public UIElement {
  public:
    /*!
     * @brief Constructor for Text.
     *
     * @param text Initial text string.
     * @param name Optional name for the element.
     */
    Text(const std::string &text = "", const std::string &name = "Text");

    /*!
     * @brief Destructor for Text.
     */
    ~Text() override = default;

    /*!
     * @brief Set the text string.
     *
     * @param text New text string.
     */
    void set_text(const std::string &text);

    /*!
     * @brief Get the text string.
     *
     * @return Current text string.
     */
    inline const std::string &get_text() const { return m_text; }

    /*!
     * @brief Set the font.
     *
     * @param font Shared pointer to the font.
     */
    void set_font(std::shared_ptr<Font> font);

    /*!
     * @brief Get the current font.
     *
     * @return Shared pointer to the font.
     */
    inline std::shared_ptr<Font> get_font() const { return m_font; }

    /*!
     * @brief Set the text color.
     *
     * @param color RGBA color (values 0.0-1.0).
     */
    inline void set_color(const glm::vec4 &color) { m_color = color; }

    /*!
     * @brief Get the text color.
     *
     * @return RGBA color.
     */
    inline const glm::vec4 &get_color() const { return m_color; }

    /*!
     * @brief Set text alignment.
     *
     * @param align Alignment option.
     */
    inline void set_alignment(TextAlign align) { m_alignment = align; }

    /*!
     * @brief Get text alignment.
     *
     * @return Current alignment.
     */
    inline TextAlign get_alignment() const { return m_alignment; }

    /*!
     * @brief Render the text.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     * @param projection Projection matrix for UI rendering.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Text string to render
    std::string m_text;

    //! @brief Font to use for rendering
    std::shared_ptr<Font> m_font;

    //! @brief Text color (RGBA)
    glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};

    //! @brief Text alignment
    TextAlign m_alignment{TextAlign::Left};

    //! @brief Shader for rendering text
    static std::shared_ptr<renderer::Shader> s_shader;

    //! @brief Vertex array for quad rendering
    static std::shared_ptr<renderer::VertexArray> s_vao;

    //! @brief Vertex buffer (dynamic)
    static std::shared_ptr<renderer::VertexBuffer> s_vbo;

    //! @brief Initialize static rendering resources
    static void initialize_resources();

    //! @brief Check if resources are initialized
    static bool s_initialized;

    /*!
     * @brief Update text size based on current text and font.
     */
    void update_size();
};

} // namespace ui

} // namespace lmgl
