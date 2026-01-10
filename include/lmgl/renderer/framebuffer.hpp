/*!
 * @file framebuffer.hpp
 * @brief Framebuffer class for off-screen rendering.
 *
 * This class encapsulates the creation and management of an OpenGL framebuffer,
 * including color and depth attachments.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}*
 */

#pragma once

#include <glad/glad.h>

#include <memory>

#include "lmgl/renderer/texture.hpp"

namespace lmgl {

namespace renderer {

/*!
 * @brief Framebuffer class for off-screen rendering.
 *
 * This class encapsulates the creation and management of an OpenGL framebuffer,
 * including color and depth attachments.
 *
 * @note The framebuffer can be resized and bound/unbound for rendering.
 */
class Framebuffer {
  public:
    /*!
     * @brief Constructor for the Framebuffer.
     *
     * Initializes the framebuffer with the specified width and height.
     * Creates a color attachment texture and a depth attachment renderbuffer.
     *
     * @param width Width of the framebuffer.
     * @param height Height of the framebuffer.
     */
    Framebuffer(int width, int height);

    //! @brief Destructor for the Framebuffer.
    ~Framebuffer();

    /*!
     * @brief Bind the framebuffer.
     *
     * Activates the framebuffer for subsequent rendering operations.
     */
    void bind() const;

    /*!
     * @brief Unbind the framebuffer.
     *
     * Deactivates the framebuffer, reverting to the default framebuffer.
     */
    void unbind() const;

    /*!
     * @brief Resize the framebuffer.
     *
     * Adjusts the size of the framebuffer and its attachments.
     *
     * @param width New width of the framebuffer.
     * @param height New height of the framebuffer.
     */
    void resize(int width, int height);

    /*!
     * @brief Get the color attachment texture.
     *
     * Returns a shared pointer to the texture used as the color attachment
     * of the framebuffer.
     *
     * @return Shared pointer to the color attachment texture.
     */
    inline std::shared_ptr<Texture> get_color_attachment() const { return m_texture_attachment; }

  private:
    /*!
     * @brief Invalidate the framebuffer.
     *
     * Recreates the framebuffer and its attachments. Called during resizing.
     */
    void invalidate();

    //! @brief Renderer ID assigned by OpenGL.
    unsigned int m_renderer_id;

    //! @brief Depth attachment renderbuffer ID.
    unsigned int m_depth_attachment;

    //! @brief Color attachment texture.
    std::shared_ptr<Texture> m_texture_attachment;

    //! @brief Width and height of the framebuffer.
    int m_width, m_height;
};

} // namespace renderer

} // namespace lmgl
