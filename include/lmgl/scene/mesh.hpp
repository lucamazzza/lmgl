/*!
 * @file mesh.hpp
 * @brief Defines the Mesh class for managing 3D mesh data.
 *
 * This header file contains the definition of the Mesh class, which encapsulates
 * a vertex array and a shader program used for rendering 3D meshes.
 * The Mesh class provides access to the underlying vertex array and shader,
 * facilitating the rendering process in a graphics application.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <memory>

namespace lmgl {

namespace scene {

/*! 
 * @brief Represents a 3D mesh with associated vertex array and shader.
 *
 * The Mesh class encapsulates a vertex array and a shader program used for rendering 3D meshes.
 * It provides access to the underlying vertex array and shader, facilitating the rendering process
 * in a graphics application.
 *
 * @note This is a placeholder implementation and may be extended with additional functionality
 * in the future.
 *
 * TODO: Extend the Mesh class with additional features as needed.
 */
class Mesh {
public:
    
    /*! 
     * @brief Constructor for the Mesh class.
     *
     * Initializes a Mesh with the provided vertex array and shader.
     *
     * @param vao Shared pointer to the VertexArray object.
     * @param shader Shared pointer to the Shader object.
     */
    Mesh(std::shared_ptr<renderer::VertexArray> vao, std::shared_ptr<renderer::Shader> shader)
     : m_vertex_array(vao), m_shader(shader) {}

    /*!
     * @brief Getter for vertex array
     *
     * Provides access to the vertex array associated with the mesh.
     *
     * @return Shared pointer to the VertexArray object.
     */
    inline std::shared_ptr<renderer::VertexArray> get_vertex_array() const { return m_vertex_array; }

    /*!
     * @brief Getter for shader
     *
     * Provides access to the shader associated with the mesh.
     *
     * @return Shared pointer to the Shader object.
     */
    inline std::shared_ptr<renderer::Shader> get_shader() const { return m_shader; }

private:

    //! @brief Vertex array associated with the mesh.
    std::shared_ptr<renderer::VertexArray> m_vertex_array;

    //! @brief Shader associated with the mesh.
    std::shared_ptr<renderer::Shader> m_shader;
};

} // namespace scene
} // namespace lmgl
