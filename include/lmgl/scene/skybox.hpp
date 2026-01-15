/*!
 * @file skybox.hpp
 * @brief Skybox rendering system with cubemap support.
 *
 * This file defines the Skybox and Cubemap classes for rendering skyboxes
 * in a 3D scene using OpenGL. The Cubemap class allows loading cubemaps from
 * either six individual face images or a single equirectangular image. The 
 * Skybox class handles rendering the skybox using a specified shader and camera.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/vertex_array.hpp"
#include "lmgl/scene/camera.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

namespace lmgl {

namespace scene {

/*!
 * @brief Represents a cubemap texture for skybox rendering.
 *
 * The Cubemap class provides functionality to load cubemaps from either six
 * individual face images or a single equirectangular image. It manages the
 * OpenGL texture ID and provides methods to bind the cubemap for rendering.
 */
class Cubemap {
  public:
    /*!
     * @brief Creates a Cubemap from six face images.
     *
     * Creates a Cubemap object by loading six images corresponding to the faces
     * of the cubemap. The images should be provided in the following order:
     * +X, -X, +Y, -Y, +Z, -Z.
     *
     * @param faces A vector of file paths to the six face images in the order: +X, -X, +Y, -Y, +Z, -Z.
     * @return A shared pointer to the created Cubemap object.
     */
    static std::shared_ptr<Cubemap> from_faces(const std::vector<std::string> &faces);

    /*!
     * @brief Creates a Cubemap from an equirectangular image.
     *
     * Creates a Cubemap object by loading a single equirectangular image and
     * converting it into a cubemap texture.
     *
     * @param path The file path to the equirectangular image.
     * @return A shared pointer to the created Cubemap object.
     */
    static std::shared_ptr<Cubemap> from_equirectangular(const std::string &path);

    /*!
     * @brief Destructor for the Cubemap class.
     *
     * Cleans up the OpenGL texture associated with the cubemap.
     */
    ~Cubemap();

    /*!
     * @brief Binds the cubemap texture to a specified texture slot.
     *
     * Binds the cubemap texture to the given texture slot for use in rendering.
     *
     * @param slot The texture slot to bind the cubemap to (default is 0).
     */
    void bind(unsigned int slot = 0) const;

    /*!
     * @brief Retrieves the OpenGL texture ID of the cubemap.
     *
     * @return The OpenGL texture ID.
     */
    inline unsigned int get_id() const { return m_renderer_id; }

  private:

    //! @brief Default constructor for the Cubemap class.
    Cubemap() = default;

    /*!
     * @brief Loads the cubemap from six face images.
     *
     * Loads the cubemap texture by reading six images corresponding to the faces
     * of the cubemap.
     *
     * @param faces A vector of file paths to the six face images.
     * @return True if loading was successful, false otherwise.
     */
    bool load_faces(const std::vector<std::string> &faces);

    /*!
     * @brief Loads the cubemap from an equirectangular image.
     *
     * Converts the equirectangular image into a cubemap texture.
     *
     * @param path The file path to the equirectangular image.
     * @return True if loading was successful, false otherwise.
     */
    bool load_equirectangular(const std::string &path);

    //! @brief OpenGL texture ID for the cubemap.
    unsigned int m_renderer_id = 0;
};

/*!
 * @brief Represents a skybox in a 3D scene.
 *
 * The Skybox class manages the rendering of a skybox using a cubemap texture.
 * It provides methods to render the skybox with a specified camera and shader.
 */
class Skybox {
  public:

    /*!
     * @brief Constructs a Skybox with the given cubemap and shader.
     *
     * Creates a Skybox object using the provided Cubemap and shader file path.
     * If no shader path is provided, a default skybox shader is used.
     *
     * @param cubemap A shared pointer to the Cubemap object to be used for the skybox.
     * @param shader_path The file path to the shader used for rendering the skybox (default is "shaders/skybox.glsl").
     */
    Skybox(std::shared_ptr<Cubemap> cubemap, const std::string &shader_path = "shaders/skybox.glsl");

    //! @brief Destructor for the Skybox class.
    ~Skybox();

    /*!
     * @brief Renders the skybox using the specified camera.
     *
     * Renders the skybox by setting up the appropriate shader uniforms and
     * drawing the cube geometry.
     *
     * @param camera A shared pointer to the Camera object used for rendering.
     */
    void render(std::shared_ptr<Camera> camera);

    /*!
     * @brief Sets the cubemap texture for the skybox.
     *
     * Sets the Cubemap object to be used for rendering the skybox.
     *
     * @param cubemap A shared pointer to the Cubemap object to be used for the skybox.
     */
    inline void set_cubemap(std::shared_ptr<Cubemap> cubemap) { m_cubemap = cubemap; }

    /*! 
     * @brief Retrieves the cubemap texture used by the skybox.
     *
     * @return A shared pointer to the Cubemap object used for the skybox.
     */
    inline std::shared_ptr<Cubemap> get_cubemap() const { return m_cubemap; }

    /*!
     * @brief Sets the exposure level for the skybox rendering.
     *
     * @param exposure The exposure level to set.
     */
    inline void set_exposure(float exposure) { m_exposure = exposure; }

    /*!
     * @brief Retrieves the exposure level for the skybox rendering.
     *
     * @return The current exposure level.
     */
    inline float get_exposure() const { return m_exposure; }

    /*!
     * @brief Retrieves the shader used for rendering the skybox.
     *
     * @return A shared pointer to the Shader object used for the skybox.
     */
    inline std::shared_ptr<renderer::Shader> get_shader() const { return m_shader; }

  private:

    /*!
     * @brief Initializes the cube geometry for the skybox.
     *
     * Sets up the vertex buffer and vertex array objects.
     */
    void init_cube_geometry();

    //! @brief Shared pointer to the Cubemap used for the skybox. 
    std::shared_ptr<Cubemap> m_cubemap;

    //! @brief Shared pointer to the Shader used for rendering the skybox.
    std::shared_ptr<renderer::Shader> m_shader;

    //! @brief Shared pointer to the VertexArray for the skybox geometry.
    std::shared_ptr<renderer::VertexArray> m_vao;

    //! @brief Exposure level for the skybox rendering.
    float m_exposure = 1.0f;

    //! @brief Vertex Buffer Object ID for the skybox geometry.
    unsigned int m_vbo = 0;
};

} // namespace lmgl

} // namespace scene
