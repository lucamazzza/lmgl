/*!
 * @file image.hpp
 * @brief Image UI element for displaying textures.
 *
 * This header file contains the definition of the Image class, which renders
 * a texture on a rectangle with optional tint color.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"
#include "lmgl/renderer/vertex_array.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace lmgl {

namespace ui {

/*!
 * @brief Image UI element for rendering textures.
 *
 * The Image class displays a texture on a rectangle with optional
 * tint color and transparency support.
 */
class Image : public UIElement {
  public:
    /*!
     * @brief Constructor for Image.
     *
     * @param name Optional name for the image.
     */
    Image(const std::string &name = "Image");

    /*!
     * @brief Destructor for Image.
     */
    ~Image() override = default;

    /*!
     * @brief Set the texture to display.
     *
     * @param texture Shared pointer to the texture.
     */
    inline void set_texture(std::shared_ptr<renderer::Texture> texture) { m_texture = texture; }

    /*!
     * @brief Get the current texture.
     *
     * @return Shared pointer to the texture.
     */
    inline std::shared_ptr<renderer::Texture> get_texture() const { return m_texture; }

    /*!
     * @brief Set the tint color for the image.
     *
     * @param color RGBA color to tint the texture (values 0.0-1.0).
     */
    inline void set_tint(const glm::vec4 &color) { m_tint = color; }

    /*!
     * @brief Get the tint color.
     *
     * @return RGBA tint color.
     */
    inline const glm::vec4 &get_tint() const { return m_tint; }

    /*!
     * @brief Render the image.
     *
     * @param canvas_width Width of the canvas in pixels.
     * @param canvas_height Height of the canvas in pixels.
     * @param projection Projection matrix for UI rendering.
     */
    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override;

  private:
    //! @brief Texture to display
    std::shared_ptr<renderer::Texture> m_texture;

    //! @brief Tint color (RGBA)
    glm::vec4 m_tint{1.0f, 1.0f, 1.0f, 1.0f};

    //! @brief Shader for rendering the image
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
