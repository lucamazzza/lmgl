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
#include "lmgl/scene/material.hpp"

#include <memory>

namespace lmgl {

namespace scene {

/*!
 * @brief Represents a single vertex in 3D space.
 *
 * The Vertex struct encapsulates the properties of a vertex, including its position,
 * normal vector, color, and texture coordinates (UVs). This structure is commonly used
 * in 3D graphics applications to define the attributes of vertices in a mesh.
 */
struct Vertex {

    //! @brief Position of the vertex in 3D space.
    glm::vec3 position;

    //! @brief Normal vector at the vertex.
    glm::vec3 normal;

    //! @brief Color of the vertex.
    glm::vec4 color;

    //! @brief Texture coordinates (UVs) of the vertex.
    glm::vec2 uvs;

    //! @brief Texture tangent
    glm::vec3 tangent;

    //! @brief Texture bitangetn
    glm::vec3 bitangent;

    /*!
     * @brief Constructor for the Vertex struct.
     *
     * Initializes a Vertex with specified position, normal, color, and UVs.
     * Default values are provided for each attribute.
     *
     * @param pos Position of the vertex (default is (0.0, 0.0, 0.0)).
     * @param norm Normal vector at the vertex (default is (0.0, 1.0, 0.0)).
     * @param col Color of the vertex (default is (1.0, 1.0, 1.0, 1.0)).
     * @param uv Texture coordinates of the vertex (default is (0.0, 0.0)).
     */
    Vertex(const glm::vec3 &pos = glm::vec3(0.0f), const glm::vec3 &norm = glm::vec3(0.0f, 1.0f, 0.0f),
           const glm::vec4 &col = glm::vec4(1.0f), const glm::vec2 &uv = glm::vec2(0.0f))
        : position(pos), normal(norm), color(col), uvs(uv) {}
};

/*!
 * @brief Represents a 3D mesh with associated vertex array and shader.
 *
 * The Mesh class encapsulates a vertex array and a shader program used for rendering 3D meshes.
 * It provides access to the underlying vertex array and shader, facilitating the rendering process
 * in a graphics application.
 */
class Mesh {
  public:
    /*!
     * @brief Constructor for the Mesh class.
     *
     * Initializes a Mesh with the provided vertices, indices, and shader.
     *
     * @param vert Vector of Vertex objects defining the mesh geometry.
     * @param indices Vector of unsigned integers defining the mesh indices.
     * @param shader Shared pointer to the Shader object.
     */
    Mesh(const std::vector<Vertex> &vert, const std::vector<unsigned int> &indices,
         std::shared_ptr<renderer::Shader> shader);

    /*!
     * @brief Constructor for the Mesh class.
     *
     * Initializes a Mesh with the provided vertex array and shader.
     *
     * @param vao Shared pointer to the VertexArray object.
     * @param shader Shared pointer to the Shader object.
     * @param idx_count Number of indices in the mesh.
     */
    Mesh(std::shared_ptr<renderer::VertexArray> vao, std::shared_ptr<renderer::Shader> shader, unsigned int idx_count);

    //! @brief Destructor for the Mesh class.
    ~Mesh() = default;

    /*!
     * @brief Binds the mesh for rendering.
     *
     * Prepares the mesh for rendering by binding the vertex array and shader.
     */
    void bind() const;

    /*!
     * @brief Unbinds the mesh after rendering.
     *
     * Cleans up after rendering by unbinding the vertex array and shader.
     */
    void unbind() const;

    /*!
     * @brief Renders the mesh.
     *
     * Issues the draw call to render the mesh using the associated vertex array and shader.
     */
    void render() const;

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

    /*!
     * @brief Setter for shader
     *
     * Sets the shader associated with the mesh.
     *
     * @param shader Shared pointer to the Shader object.
     */
    inline void set_shader(std::shared_ptr<renderer::Shader> shader) { m_shader = shader; }

    /*!
     * @brief Getter for index count
     *
     * Provides the number of indices in the mesh.
     *
     * @return Number of indices as an unsigned int.
     */
    inline unsigned int get_index_count() const { return m_index_count; }

    /*!
     * @brief Getter for vertices
     *
     * Provides access to the vector of vertices defining the mesh geometry.
     *
     * @return Constant reference to the vector of Vertex objects.
     */
    inline const std::vector<Vertex> &get_vertices() const { return m_vertices; }

    /*!
     * @brief Getter for indices
     *
     * Provides access to the vector of indices defining the mesh geometry.
     *
     * @return Constant reference to the vector of unsigned int indices.
     */
    inline const std::vector<unsigned int> &get_indices() const { return m_indices; }

    /*!
     * @brief Check if the mesh has vertex data
     *
     * Determines if the mesh contains valid vertex and index data.
     *
     * @return True if both vertices and indices are present, false otherwise.
     */
    inline bool has_vert_data() const { return !m_vertices.empty() && !m_indices.empty(); }

    /*!
     * @brief Retrieve the material associated with the mesh
     *
     * @return the material associated to the mesh.
     */
    inline std::shared_ptr<Material> get_material() const { return m_material; }

    /*!
     * @brief Sets the material associated to the mesh
     *
     * @param material The new material to set.
     */
    inline void set_material(std::shared_ptr<Material> material) { m_material = material; }
    // Factory Methods

    /*!
     * @brief Creates a cube mesh.
     *
     * Generates a cube mesh with the specified shader and subdivisions.
     *
     * @param shader Shared pointer to the Shader object.
     * @param subdivs Number of subdivisions for the cube (default is 1).
     * @return Shared pointer to the created Mesh object.
     */
    static std::shared_ptr<Mesh> create_cube(std::shared_ptr<renderer::Shader> shader, unsigned int subdivs = 1);

    /*!
     * @brief Creates a quad mesh.
     *
     * Generates a quad mesh with the specified shader, width, and height.
     *
     * @param shader Shared pointer to the Shader object.
     * @param width Width of the quad (default is 1.0f).
     * @param height Height of the quad (default is 1.0f).
     * @return Shared pointer to the created Mesh object.
     */
    static std::shared_ptr<Mesh> create_quad(std::shared_ptr<renderer::Shader> shader, float width = 1.0f,
                                             float height = 1.0f);

    /*!
     * @brief Creates a sphere mesh.
     *
     * Generates a sphere mesh with the specified shader, radius, and segmentation.
     *
     * @param shader Shared pointer to the Shader object.
     * @param radius Radius of the sphere (default is 0.5f).
     * @param lonsegs Number of longitudinal segments (default is 32).
     * @param latsegs Number of latitudinal segments (default is 32).
     * @return Shared pointer to the created Mesh object.
     */
    static std::shared_ptr<Mesh> create_sphere(std::shared_ptr<renderer::Shader> shader, float radius = 0.5f,
                                               unsigned int lonsegs = 32, unsigned int latsegs = 32);

  private:
    //! @brief Material associated to the mesh.
    std::shared_ptr<Material> m_material;

    //! @brief Vertex array associated with the mesh.
    std::shared_ptr<renderer::VertexArray> m_vertex_array;

    //! @brief Shader associated with the mesh.
    std::shared_ptr<renderer::Shader> m_shader;

    //! @brief Number of indices in the mesh.
    unsigned int m_index_count;

    //! @brief Vector of vertices defining the mesh geometry.
    std::vector<Vertex> m_vertices;

    //! @brief Vector of indices defining the mesh geometry.
    std::vector<unsigned int> m_indices;

    /*!
     * @brief Sets up the mesh by creating the vertex array.
     *
     * Initializes the vertex array using the stored vertices and indices.
     */
    void setup_mesh();
};

} // namespace scene
} // namespace lmgl
