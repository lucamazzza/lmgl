/*!
 * @file material.hpp
 * @brief Definition of the Material class for 3D rendering.
 *
 * This file defines the Material class, which encapsulates properties
 * and textures used in 3D rendering, such as albedo, metallic, roughness,
 * ambient occlusion, and emissive characteristics.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace lmgl {

namespace scene {

/**
 * @class Material
 * @brief Represents a material with various properties and textures for 3D rendering.
 *
 * The Material class encapsulates properties such as albedo, metallic, roughness,
 * ambient occlusion, and emissive characteristics, along with their corresponding
 * texture maps. It provides methods to set and retrieve these properties and bind
 * them to a shader for rendering.
 */
class Material {
  public:
    /*!
     * @brief Constructs a Material with an optional name.
     *
     * Creates a Material object with the specified name. If no name is provided,
     * it defaults to "Default Material".
     *
     * @param name The name of the material.
     */
    Material(const std::string &name = "Default Material");

    /*!
     * @brief Gets the name of the material.
     *
     * @return The name of the material.
     */
    inline const std::string &get_name() const { return m_name; }

    /*!
     * @brief Sets the name of the material.
     *
     * @param name The new name for the material.
     */
    inline void set_name(const std::string &name) { m_name = name; }

    /*!
     * @brief Gets the albedo color of the material.
     *
     * @return The albedo color as a glm::vec3.
     */
    inline const glm::vec3 &get_albedo() const { return m_albedo; }

    /*!
     * @brief Sets the albedo color of the material.
     *
     * @param albedo The new albedo color as a glm::vec3.
     */
    inline void set_albedo(const glm::vec3 &albedo) { m_albedo = albedo; }

    /*!
     * @brief Gets the metallic property of the material.
     *
     * @return The metallic value as a float.
     */
    inline float get_metallic() const { return m_metallic; }

    /*!
     * @brief Sets the metallic property of the material.
     *
     * @param metallic The new metallic value as a float.
     */
    inline void set_metallic(float metallic) { m_metallic = metallic; }

    /*!
     * @brief Gets the roughness property of the material.
     *
     * @return The roughness value as a float.
     */
    inline float get_roughness() const { return m_roughness; }

    /*!
     * @brief Sets the roughness property of the material.
     *
     * @param roughness The new roughness value as a float.
     */
    inline void set_roughness(float roughness) { m_roughness = roughness; }

    /*!
     * @brief Gets the ambient occlusion (AO) property of the material.
     *
     * @return The AO value as a float.
     */
    inline float get_ao() const { return m_ao; }

    /*!
     * @brief Sets the ambient occlusion (AO) property of the material.
     *
     * @param ao The new AO value as a float.
     */
    inline void set_ao(float ao) { m_ao = ao; }

    /*!
     * @brief Gets the emissive color of the material.
     *
     * @return The emissive color as a glm::vec3.
     */
    inline const glm::vec3 &get_emissive() const { return m_emissive; }

    /*!
     * @brief Sets the emissive color of the material.
     *
     * @param emissive The new emissive color as a glm::vec3.
     */
    inline void set_emissive(const glm::vec3 &emissive) { m_emissive = emissive; }

    /*!
     * @brief Sets the albedo texture map of the material.
     *
     * @param texture A shared pointer to the albedo texture.
     */
    void set_albedo_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Sets the normal texture map of the material.
     *
     * @param texture A shared pointer to the normal texture.
     */
    void set_normal_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Sets the metallic texture map of the material.
     *
     * @param texture A shared pointer to the metallic texture.
     */
    void set_metallic_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Sets the roughness texture map of the material.
     *
     * @param texture A shared pointer to the roughness texture.
     */
    void set_roughness_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Sets the ambient occlusion (AO) texture map of the material.
     *
     * @param texture A shared pointer to the AO texture.
     */
    void set_ao_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Sets the emissive texture map of the material.
     *
     * @param texture A shared pointer to the emissive texture.
     */
    void set_emissive_map(const std::shared_ptr<renderer::Texture> &texture);

    /*!
     * @brief Gets the albedo texture map of the material.
     *
     * @return A shared pointer to the albedo texture.
     */
    inline std::shared_ptr<renderer::Texture> get_albedo_map() const { return m_albedo_map; };

    /*!
     * @brief Gets the normal texture map of the material.
     *
     * @return A shared pointer to the normal texture.
     */
    inline std::shared_ptr<renderer::Texture> get_normal_map() const { return m_normal_map; };

    /*!
     * @brief Gets the metallic texture map of the material.
     *
     * @return A shared pointer to the metallic texture.
     */
    inline std::shared_ptr<renderer::Texture> get_metallic_map() const { return m_metallic_map; };

    /*!
     * @brief Gets the roughness texture map of the material.
     *
     * @return A shared pointer to the roughness texture.
     */
    inline std::shared_ptr<renderer::Texture> get_roughness_map() const { return m_roughness_map; };

    /*!
     * @brief Gets the ambient occlusion (AO) texture map of the material.
     *
     * @return A shared pointer to the AO texture.
     */
    inline std::shared_ptr<renderer::Texture> get_ao_map() const { return m_ao_map; };

    /*!
     * @brief Gets the emissive texture map of the material.
     *
     * @return A shared pointer to the emissive texture.
     */
    inline std::shared_ptr<renderer::Texture> get_emissive_map() const { return m_emissive_map; };

    /*!
     * @brief Binds the material properties and textures to the given shader.
     *
     * This method sets the material properties and binds the associated texture maps
     * to the provided shader for rendering.
     *
     * @param shader A shared pointer to the shader to bind the material to.
     */
    void bind(const std::shared_ptr<renderer::Shader> &shader) const;

  private:
    //! @brief The name of the material.
    std::string m_name;

    //! @brief Material albedo color.
    glm::vec3 m_albedo{1.0f, 1.0f, 1.0f};

    //! @brief Material metallic property.
    float m_metallic{0.0f};

    //! @brief Material roughness property.
    float m_roughness{0.5f};

    //! @brief Material ambient occlusion (AO) property.
    float m_ao{1.0f};

    //! @brief Material emissive color.
    glm::vec3 m_emissive{0.0f, 0.0f, 0.0f};

    //! @brief Albedo texture map.
    std::shared_ptr<renderer::Texture> m_albedo_map;

    //! @brief Normal texture map.
    std::shared_ptr<renderer::Texture> m_normal_map;

    //! @brief Metallic texture map.
    std::shared_ptr<renderer::Texture> m_metallic_map;

    //! @brief Roughness texture map.
    std::shared_ptr<renderer::Texture> m_roughness_map;

    //! @brief Ambient occlusion (AO) texture map.
    std::shared_ptr<renderer::Texture> m_ao_map;

    //! @brief Emissive texture map.
    std::shared_ptr<renderer::Texture> m_emissive_map;
};

} // namespace scene

} // namespace lmgl
