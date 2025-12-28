/*!
 * @file texture.hpp
 * @brief Texture class for managing OpenGL textures.
 *
 * This class provides functionality to create, bind, unbind,
 * resize, and set data for OpenGL textures.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include <glad/glad.h>

#include <string>

namespace lmgl {

namespace renderer {

/*!
 * @brief Manages an OpenGL texture.
 *
 * This class encapsulates the creation, binding, unbinding,
 * resizing, and data management of an OpenGL texture.
 * It provides methods to interact with the texture in a rendering context.
 */
class Texture {
public:

    /*! 
     * @brief Constructor for creating a texture with specified width and height.
     *
     * Initializes an empty texture with the given dimensions.
     *
     * @param width The width of the texture.
     * @param height The height of the texture.
     */
    Texture(int width, int height);

    /*! 
     * @brief Constructor for creating a texture from an image file.
     *
     * Loads the texture data from the specified file path.
     *
     * @param fpath The file path to the image to load as a texture.
     */
    Texture(const std::string& fpath);

    //! @brief Destructor for the Texture class.
    ~Texture();

    /*! 
     * @brief Binds the texture to a specified texture slot.
     *
     * Activates the texture for use in rendering operations.
     *
     * @param slot The texture slot to bind the texture to (default is 0).
     */
    void bind(unsigned int slot = 0) const;

    /*! 
     * @brief Unbinds the texture.
     *
     * Deactivates the texture from the current texture slot.
     */
    void unbind() const;

    /*! 
     * @brief Resizes the texture to new dimensions.
     *
     * Updates the texture's width and height.
     *
     * @param width The new width of the texture.
     * @param height The new height of the texture.
     */
    void resize(int width, int height);

    /*! 
     * @brief Gets the width of the texture.
     *
     * @return The width of the texture.
     */
    inline int get_width() const { return m_width; }

    /*! 
     * @brief Gets the height of the texture.
     *
     * @return The height of the texture.
     */
    inline int get_height() const { return m_height; }

    /*! 
     * @brief Gets the OpenGL renderer ID of the texture.
     *
     * @return The renderer ID of the texture.
     */
    inline unsigned int get_id() const { return m_renderer_id; }

    /*! 
     * @brief Sets the data of the texture.
     *
     * Updates the texture's data with the provided data buffer.
     *
     * @param data Pointer to the new texture data.
     * @param size Size of the new texture data in bytes.
     */
    void set_data(void* data, unsigned int size);

private:

    //! @brief OpenGL renderer ID for the texture.
    unsigned int m_renderer_id;

    //! @brief File path of the texture image (if loaded from file).
    std::string m_file_path;

    //! @brief Width and height of the texture.
    int m_width, m_height;

    //! @brief Internal format and data format of the texture.
    GLenum m_internal_format;

    //! @brief Internal format and data format of the texture.
    GLenum m_data_format;

    /*! 
     * @brief Initializes texture parameters.
     *
     * Sets default parameters for the texture such as filtering and wrapping modes.
     */
    void init_texture_params();
};

} // namespace renderer

} // namespace lmgl
