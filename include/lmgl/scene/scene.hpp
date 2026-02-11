/*!
 * @file scene.hpp
 * @brief Defines the Scene class for managing a 3D scene graph.
 *
 * This header file contains the definition of the Scene class, which represents
 * a 3D scene graph with a root node. The Scene class provides methods to
 * update the scene and access the root node.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/scene/light.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/skybox.hpp"

#include <memory>
#include <string>

namespace lmgl {

/*!
 * @brief Namespace for scene-related classes and functions.
 *
 * This namespace contains classes and functions related to scene management,
 * including nodes, meshes, and transformations.
 */
namespace scene {

/*!
 * @brief Represents a 3D scene graph with a root node.
 *
 * The Scene class encapsulates a 3D scene graph, providing a root node
 * from which all other nodes in the scene can be accessed. It also
 * provides methods to update the scene.
 */
class Scene {
  public:
    /*!
     * @brief Constructor for the Scene class.
     *
     * Initializes a Scene with a root node.
     *
     * @param name Optional name for the scene.
     */
    Scene(const std::string &name = "Scene");

    /*!
     * @brief Getter for the root node.
     *
     * Provides access to the root node of the scene graph.
     *
     * @return Shared pointer to the root Node object.
     */
    inline std::shared_ptr<Node> get_root() const { return m_root; }

    /*!
     * @brief Update the scene.
     *
     * This method updates the scene, typically by updating the root node
     * and propagating changes through the scene graph.
     */
    void update();

    /*!
     * @brief Getters and setters for the scene's name.
     *
     * Provides access to the scene's name property.
     *
     * @return Name of the scene.
     */
    inline const std::string &get_name() const { return m_name; }

    /*!
     * @brief Set the name of the scene.
     *
     * Updates the scene's name property.
     *
     * @param name New name for the scene.
     */
    inline void set_name(const std::string &name) { m_name = name; }

    /*!
     * @brief Returns all the lights in the scene.
     *
     * @return Vector containing all the lights introduced into the scene.
     */
    inline std::vector<std::shared_ptr<Light>> get_lights() const { return m_lights; }

    /*!
     * @brief Adds a light in the scene.
     *
     * @param light the light to add.
     */
    void add_light(std::shared_ptr<Light> light);

    /*!
     * @brief Removes a light from the scene.
     *
     * @param light the light to remove.
     */
    void remove_light(std::shared_ptr<Light> light);

    /*!
     * @brief Removes all lights from the scene.
     */
    void clear_lights();

    /*!
     * @brief Set the skybox for the scene.
     *
     * @param skybox The skybox to set (or nullptr to remove).
     */
    inline void set_skybox(std::shared_ptr<Skybox> skybox) { m_skybox = skybox; }

    /*!
     * @brief Get the skybox of the scene.
     *
     * @return The skybox (or nullptr if none).
     */
    inline std::shared_ptr<Skybox> get_skybox() const { return m_skybox; }

    /*!
     * @brief Enable or disable shadows for the scene.
     *
     * When enabled, the scene will automatically render shadow maps
     * for directional lights.
     *
     * @param enabled True to enable shadows, false to disable.
     */
    void set_shadows_enabled(bool enabled);

    /*!
     * @brief Check if shadows are enabled.
     *
     * @return True if shadows are enabled.
     */
    inline bool are_shadows_enabled() const { return m_shadows_enabled; }

    /*!
     * @brief Set shadow map resolution.
     *
     * @param resolution Resolution for shadow maps (e.g., 2048, 4096).
     */
    void set_shadow_resolution(int resolution);

    /*!
     * @brief Get the shadow map resolution.
     *
     * @return Shadow map resolution.
     */
    inline int get_shadow_resolution() const { return m_shadow_resolution; }

  private:
    //! @brief Name of the scene.
    std::string m_name;

    //! @brief Root node of the scene graph.
    std::shared_ptr<Node> m_root;

    //! @brief Lights in the scene.
    std::vector<std::shared_ptr<Light>> m_lights;

    //! @brief Skybox for the scene.
    std::shared_ptr<Skybox> m_skybox;

    //! @brief Whether shadows are enabled.
    bool m_shadows_enabled = true;

    //! @brief Shadow map resolution.
    int m_shadow_resolution = 2048;
};

} // namespace scene

} // namespace lmgl
