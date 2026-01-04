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

#include "lmgl/scene/node.hpp"

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
    Scene(const std::string& name = "Scene");

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
    inline const std::string& get_name() const { return m_name; }

    /*! 
     * @brief Set the name of the scene.
     *
     * Updates the scene's name property.
     *
     * @param name New name for the scene.
     */
    inline void set_name(const std::string& name) { m_name = name; }

private:

    //! @brief Name of the scene.
    std::string m_name;

    //! @brief Root node of the scene graph.
    std::shared_ptr<Node> m_root;
};

} // namespace scene

} // namespace lmgl
