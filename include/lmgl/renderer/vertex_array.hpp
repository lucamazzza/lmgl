/*!
 * @file vertex_array.hpp 
 *
 * @brief Declaration of the VertexArray class for managing Vertex Array Objects (VAOs) in OpenGL.
 *
 * This file contains the declaration of the VertexArray class, which encapsulates
 * the functionality of a Vertex Array Object (VAO) in OpenGL. The class provides methods
 * to bind and unbind the VAO, add vertex buffers, and set an index buffer.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "buffer.hpp"

#include <memory>
#include <vector>

namespace lmgl {

namespace renderer {

/*!
 * @brief Class representing a Vertex Array Object (VAO) in OpenGL.
 *
 * This class encapsulates the functionality of a VAO, allowing for the management
 * of vertex and index buffers used in rendering operations.
 *
 * @note This class assumes the existence of VertexBuffer and IndexBuffer classes
 * defined in "buffer.hpp".
 */
class VertexArray {
public:

    //! Constructs a new Vertex Array Object (VAO)
    VertexArray();

    //! Destructor to clean up the VAO
    ~VertexArray();

    /*!
     * @brief Binds the Vertex Array Object for rendering.
     * 
     * This method makes the VAO active so that subsequent rendering commands use it.
     *
     * @note Ensure to call unbind() after rendering to avoid unintended side effects.
     */
    void bind() const;

    /*!
     * @brief Unbinds the Vertex Array Object.
     * 
     * This method deactivates the VAO, preventing it from being used in subsequent rendering commands.
     *
     * @note It is good practice to unbind the VAO after rendering to maintain a clean state.
     */
    void unbind() const;

    /*!
     * @brief Adds a Vertex Buffer to the Vertex Array Object.
     *
     * This method associates a Vertex Buffer with the VAO, allowing it to be used for rendering.
     *
     * @param vertexBuffer A shared pointer to the Vertex Buffer to be added.
     */
    void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer);

    /*!
     * @brief Sets the Index Buffer for the Vertex Array Object.
     *
     * This method associates an Index Buffer with the VAO, enabling indexed rendering.
     *
     * @param indexBuffer A shared pointer to the Index Buffer to be set.
     */
    void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer);

    /*! 
     * @brief Retrieves the list of Vertex Buffers associated with the Vertex Array Object.
     *
     * This method returns a constant reference to a vector containing shared pointers to the Vertex Buffers.
     *
     * @return A constant reference to a vector of shared pointers to the Vertex Buffers.
     */
    const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const;

    /*! 
     * @brief Retrieves the Index Buffer associated with the Vertex Array Object.
     *
     * This method returns a shared pointer to the Index Buffer.
     *
     * @return A shared pointer to the Index Buffer.
     */
    const std::shared_ptr<IndexBuffer>& get_index_buffer() const;

private:

    //! Renderer ID for the Vertex Array Object
    unsigned int m_renderer_id;

    //! List of Vertex Buffers associated with the VAO
    std::vector<std::shared_ptr<VertexBuffer>> m_vertex_buffers;

    //! Index Buffer associated with the VAO
    std::shared_ptr<IndexBuffer> m_index_buffer;
};

} // namespace renderer

} // namespace lmgl
