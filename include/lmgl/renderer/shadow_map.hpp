/*!
 * @file shadow_map.hpp
 * @brief This file contains the ShadowMap, CubemapShadowMap, and ShadowRenderer classes for rendering shadows in a 3D
 * scene.
 *
 * ShadowMap class manages a 2D depth texture for directional light shadows.
 * CubemapShadowMap class manages a cubemap depth texture for point light shadows.
 * ShadowRenderer class provides methods to render shadows for directional and point lights using the respective shadow
 * maps.
 *
 * \copyright{Copyright (c) 2024 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */
#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/scene/light.hpp"
#include "lmgl/scene/scene.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>

namespace lmgl {

namespace renderer {

/*!
 * @brief Manages a 2D depth texture for rendering shadows from directional lights.
 *
 * This class encapsulates the creation and management of a framebuffer object (FBO)
 * and a depth texture used for shadow mapping. It provides methods to bind and unbind
 * the FBO, bind the depth texture for use in shaders, and resize the shadow map.
 *
 * @note The default resolution is set to 2048x2048 pixels.
 */
class ShadowMap {
  public:
    /*!
     * @brief Constructs a ShadowMap with the specified width and height.
     *
     * Creates a framebuffer object and a depth texture for shadow mapping.
     *
     * @param width The width of the shadow map texture. Default is 2048.
     * @param height The height of the shadow map texture. Default is 2048.
     */
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);

    //! @brief Destructor for ShadowMap.
    ~ShadowMap();

    /*!
     * @brief Binds the framebuffer object for rendering to the shadow map.
     *
     * This method binds the FBO so that subsequent rendering commands
     * will write depth information to the shadow map texture.
     *
     * @note Make sure to unbind the FBO after rendering using the `unbind` method.
     */
    void bind();

    /*!
     * @brief Unbinds the framebuffer object, returning to the default framebuffer.
     *
     * This method should be called after rendering to the shadow map is complete.
     * It restores rendering to the default framebuffer.
     */
    void unbind();

    /*!
     * @brief Binds the depth texture to the specified texture slot for use in shaders.
     *
     * This method binds the depth texture so that it can be sampled in shaders
     * for shadow calculations.
     *
     * @param slot The texture slot to bind the depth texture to. Default is 0.
     */
    void bind_texture(unsigned int slot = 0) const;

    /*!
     * @brief Returns the OpenGL texture ID of the depth texture.
     *
     * @return The texture ID of the depth texture.
     */
    inline unsigned int get_texture_id() const { return m_depth_map; }

    /*!
     * @brief Returns the width of the shadow map texture.
     *
     * @return The width of the shadow map texture in pixels.
     */
    inline unsigned int get_width() const { return m_width; }

    /*!
     * @brief Returns the height of the shadow map texture.
     *
     * @return The height of the shadow map texture in pixels.
     */
    inline unsigned int get_height() const { return m_height; }

    /*!
     * @brief Resizes the shadow map texture to the specified width and height.
     *
     * This method recreates the depth texture with the new dimensions.
     *
     * @param width The new width of the shadow map texture.
     * @param height The new height of the shadow map texture.
     */
    void resize(unsigned int width, unsigned int height);

  private:
    //! Framebuffer object ID
    unsigned int m_fbo = 0;

    //! Depth texture ID
    unsigned int m_depth_map = 0;

    //! Width of the shadow map texture
    unsigned int m_width;

    //! Height of the shadow map texture
    unsigned int m_height;
};

/*!
 * @brief Manages a cubemap depth texture for rendering shadows from point lights.
 *
 * This class encapsulates the creation and management of a framebuffer object (FBO)
 * and a cubemap depth texture used for shadow mapping with point lights. It provides
 * methods to bind and unbind the FBO, bind the cubemap depth texture for use in shaders,
 * and retrieve the texture ID and resolution.
 *
 * @note The default resolution is set to 1024x1024 pixels per face.
 */
class CubemapShadowMap {
  public:
    /*!
     * @brief Constructs a CubemapShadowMap with the specified resolution.
     *
     * Creates a framebuffer object and a cubemap depth texture for shadow mapping.
     *
     * @param resolution The resolution of each face of the cubemap shadow map. Default is 1024.
     */
    CubemapShadowMap(unsigned int resolution = 1024);

    //! @brief Destructor for CubemapShadowMap.
    ~CubemapShadowMap();

    /*!
     * @brief Binds the framebuffer object for rendering to a specific face of the cubemap shadow map.
     *
     * This method binds the FBO so that subsequent rendering commands
     * will write depth information to the specified face of the cubemap shadow map texture.
     *
     * @param face The index of the cubemap face to bind (0-5).
     *
     * @note Make sure to unbind the FBO after rendering using the `unbind` method.
     */
    void bind(unsigned int face);

    /*!
     * @brief Unbinds the framebuffer object, returning to the default framebuffer.
     *
     * This method should be called after rendering to the cubemap shadow map is complete.
     * It restores rendering to the default framebuffer.
     */
    void unbind();

    /*!
     * @brief Binds the cubemap depth texture to the specified texture slot for use in shaders.
     *
     * This method binds the cubemap depth texture so that it can be sampled in shaders
     * for shadow calculations.
     *
     * @param slot The texture slot to bind the cubemap depth texture to. Default is 0.
     */
    void bind_texture(unsigned int slot = 0) const;

    /*!
     * @brief Returns the OpenGL texture ID of the cubemap depth texture.
     *
     * @return The texture ID of the cubemap depth texture.
     */
    inline unsigned int get_texture_id() const { return m_depth_cubemap; }

    /*!
     * @brief Returns the resolution of each face of the cubemap shadow map.
     *
     * @return The resolution of each face in pixels.
     */
    inline unsigned int get_resolution() const { return m_resolution; }

  private:
    //! Framebuffer object ID
    unsigned int m_fbo = 0;

    //! Cubemap depth texture ID
    unsigned int m_depth_cubemap = 0;

    //! Resolution of each face of the cubemap shadow map
    unsigned int m_resolution;
};

/*!
 * @brief Renders shadows for directional and point lights using shadow maps.
 *
 * This class provides methods to render shadows for both directional lights
 * and point lights using the respective shadow maps. It utilizes depth shaders
 * to render the scene from the light's perspective and generate the shadow maps.
 */
class ShadowRenderer {
  public:
    /*!
     * @brief Constructs a ShadowRenderer and initializes depth shaders.
     *
     * Initializes the depth shaders used for rendering shadows.
     * The default constructor sets up the necessary shaders for both
     * directional and point light shadow mapping.
     */
    ShadowRenderer();

    //! @brief Destructor for ShadowRenderer.
    ~ShadowRenderer() = default;

    /*!
     * @brief Renders the shadow map for a directional light.
     *
     * This method renders the scene from the perspective of the directional light
     * and populates the provided shadow map with depth information.
     *
     * @param scene The scene to render.
     * @param light The directional light casting shadows.
     * @param shadow_map The shadow map to populate with depth information.
     */
    void render_directional_shadow(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Light> light,
                                   std::shared_ptr<ShadowMap> shadow_map);

    /*!
     * @brief Renders the shadow map for a point light.
     *
     * This method renders the scene from the perspective of the point light
     * and populates the provided cubemap shadow map with depth information.
     *
     * @param scene The scene to render.
     * @param light The point light casting shadows.
     * @param shadow_map The cubemap shadow map to populate with depth information.
     */
    void render_point_shadow(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Light> light,
                             std::shared_ptr<ShadowMap> shadow_map);

    /*!
     * @brief Computes the light space matrix for a directional light.
     *
     * This method calculates the light space transformation matrix used
     * for shadow mapping based on the light's direction and the scene's bounding sphere.
     *
     * @param light The directional light.
     * @param scene_center The center of the scene's bounding sphere.
     * @param scene_radius The radius of the scene's bounding sphere.
     * @return The computed light space matrix.
     */
    glm::mat4 get_light_space_matrix(std::shared_ptr<scene::Light> light, const glm::vec3 &scene_center,
                                     float scene_radius);

  private:

    //! Depth shader for directional light shadow mapping
    std::shared_ptr<Shader> m_depth_shader;

    //! Depth shader for point light shadow mapping
    std::shared_ptr<Shader> m_depth_cubemap_shader;

    /*!
     * @brief Renders the scene to populate the depth information for shadow mapping.
     *
     * This private method is used internally to render the scene from the light's
     * perspective and generate depth information for shadow mapping.
     *
     * @param scene The scene to render.
     * @param light_space_matrix The light space transformation matrix.
     */
    void render_scene_depth(std::shared_ptr<scene::Scene> scene, const glm::mat4 &light_space_matrix);
};

} // namespace renderer

} // namespace lmgl
