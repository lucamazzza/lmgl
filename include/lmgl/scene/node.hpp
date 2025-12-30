/*!
 * @file node.hpp
 * @brief Defines the Node class for managing scene graph nodes in a 3D environment.
 *
 * This header file contains the definition of the Node class, which represents
 * a node in a scene graph. Each node can have a position, rotation, scale,
 * and can contain child nodes, forming a hierarchical structure.
 * Nodes can also hold a reference to a Mesh object.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}* 
 */

#pragma once

#include "lmgl/scene/mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <memory>
#include <string>

namespace lmgl {

namespace scene {

/*! 
 * @brief Represents a node in a scene graph.
 *
 * The Node class encapsulates the properties and behaviors of a node in a 3D scene graph.
 * Each node can have a position, rotation, scale, and can contain child nodes,
 * forming a hierarchical structure. Nodes can also hold a reference to a Mesh object.
 *
 * @note The class uses glm for vector and matrix operations.
 */
class Node : public std::enable_shared_from_this<Node> {
public:
    
    // Transforms

    /*! 
     * @brief Constructor for the Node class.
     *
     * Initializes a Node with default position, rotation, and scale.
     *
     * @param name Optional name for the node.
     */
    Node(const std::string& name = "Node");
    
    //! @brief Destructor for the Node class.
    ~Node() = default;

    /*! 
     * @brief Set the position of the node.
     *
     * Updates the node's position in 3D space.
     *
     * @param position New position as a glm::vec3.
     */
    void set_position(const glm::vec3& position);

    /*! 
     * @brief Set the rotation of the node.
     *
     * Updates the node's rotation using a quaternion.
     *
     * @param rotation New rotation as a glm::quat.
     */
    void set_rotation(const glm::quat& rotation);

    /*! 
     * @brief Set the rotation of the node using Euler angles.
     *
     * Updates the node's rotation based on Euler angles (in radians).
     *
     * @param euler_angles New rotation as a glm::vec3 representing Euler angles.
     */
    void set_rotation(const glm::vec3& euler_angles);

    /*! 
     * @brief Set the scale of the node.
     *
     * Updates the node's scale in 3D space.
     *
     * @param scale New scale as a glm::vec3.
     */
    void set_scale(const glm::vec3& scale);

    /*! 
     * @brief Set the uniform scale of the node.
     *
     * Updates the node's scale uniformly in all dimensions.
     *
     * @param scale New uniform scale as a float.
     */
    void set_scale(float scale);

    /*! 
     * @brief Getters for position, rotation, scale, and transforms.
     *
     * Provides access to the node's position, rotation, scale,
     * local transform, and world transform.
     *
     * @return Corresponding property of the node.
     */
    inline const glm::vec3& get_position() const { return m_position; }

    /*! 
     * @brief Get the rotation of the node.
     *
     * Provides access to the node's rotation as a quaternion.
     *
     * @return Rotation of the node.
     */
    inline const glm::quat& get_rotation() const { return m_rotation; }

    /*! 
     * @brief Get the scale of the node.
     *
     * Provides access to the node's scale.
     *
     * @return Scale of the node.
     */
    inline const glm::vec3& get_scale() const { return m_scale; }

    /*! 
     * @brief Rotate the node around a specified axis.
     *
     * Applies a rotation to the node by a given angle around the specified axis.
     *
     * @param angle Rotation angle in radians.
     * @param axis Axis of rotation as a glm::vec3.
     */
    void rotate(float angle, const glm::vec3& axis);

    /*! 
     * @brief Get the Euler angles of the node's rotation.
     *
     * Converts the node's quaternion rotation to Euler angles (in radians).
     *
     * @return Euler angles as a glm::vec3.
     */
    glm::vec3 get_euler_angles() const;

    /*! 
     * @brief Make the node look at a target point.
     *
     * Adjusts the node's rotation so that it faces the specified target point.
     *
     * @param target Target point to look at as a glm::vec3.
     * @param up Up direction for the node (default is world up).
     */
    void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

    /*! 
     * @brief Get the local and world transforms of the node.
     *
     * Provides access to the node's local and world transformation matrices.
     *
     * @return Corresponding transformation matrix.
     */
    inline glm::mat4 get_local_transform() const { return m_local_transform; }
    inline glm::mat4 get_world_transform() const { return m_world_transform; }

    // Hierarchy

    /*! 
     * @brief Manage child nodes.
     *
     * Methods to add and remove child nodes, retrieve the parent node,
     * get the list of child nodes, and detach the node from its parent.
     *
     * @param child Child node to be added or removed.
     */
    void add_child(std::shared_ptr<Node> child);

    /*! 
     * @brief Remove a child node.
     *
     * Detaches the specified child node from this node.
     *
     * @param child Child node to be removed.
     */
    void remove_child(std::shared_ptr<Node> child);

    /*! 
     * @brief Getters for parent and children.
     *
     * Provides access to the parent node and the list of child nodes.
     *
     * @return Corresponding node or list of nodes.
     */
    inline std::shared_ptr<Node> get_parent() const { return m_parent.lock(); }

    /*! 
     * @brief Get the list of child nodes.
     *
     * Provides access to the vector of child nodes.
     *
     * @return Vector of shared pointers to child nodes.
     */
    inline const std::vector<std::shared_ptr<Node>>& get_children() const { return m_children; }

    /*! 
     * @brief Detach the node from its parent.
     *
     * Removes this node from its parent's list of children.
     */
    void detach_from_parent();

    /*! 
     * @brief Update transforms.
     *
     * Updates the local and world transformation matrices of the node.
     * This method should be called whenever the node's position, rotation,
     * or scale changes, or when the parent's transform is updated.
     *
     * @param par_transform Parent's world transform matrix (default is identity).
     */
    void update_transform(const glm::mat4& par_transform = glm::mat4(1.0f));

    /*! 
     * @brief Manage the mesh associated with the node.
     *
     * Methods to set, get, and check for the presence of a Mesh object.
     *
     * @param mesh Shared pointer to the Mesh object.
     */
    inline void set_mesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }

    /*! 
     * @brief Get the mesh associated with the node.
     *
     * Provides access to the Mesh object held by the node.
     *
     * @return Shared pointer to the Mesh object.
     */
    inline std::shared_ptr<Mesh> get_mesh() const { return m_mesh; }

    /*! 
     * @brief Check if the node has an associated mesh.
     *
     * Returns true if the node has a Mesh object, false otherwise.
     *
     * @return Boolean indicating presence of a Mesh.
     */
    inline bool has_mesh() const { return m_mesh != nullptr; }

    /*! 
     * @brief Getters and setters for the node's name.
     *
     * Provides access to the node's name property.
     *
     * @return Name of the node.
     */
    inline std::string& get_name() { return m_name; }

    /*! 
     * @brief Set the name of the node.
     *
     * Updates the node's name property.
     *
     * @param name New name for the node.
     */
    inline void set_name(const std::string& name) { m_name = name; }

private:

    //! @brief Node properties
    std::string m_name;

    //! @brief Transform properties
    glm::vec3 m_position{0.0f, 0.0f, 0.0f};

    //! @brief Rotation represented as a quaternion
    glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f};

    //! @brief Scale of the node
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    //! @brief Transformation matrices
    glm::mat4 m_local_transform{1.0f};

    //! @brief World transformation matrix
    glm::mat4 m_world_transform{1.0f};

    //! @brief Hierarchy properties
    std::weak_ptr<Node> m_parent;

    //! @brief List of child nodes
    std::vector<std::shared_ptr<Node>> m_children;

    //! @brief Mesh associated with the node
    std::shared_ptr<Mesh> m_mesh;

    /*! 
     * @brief Update the local transformation matrix.
     *
     * Recalculates the local transformation matrix based on the current
     * position, rotation, and scale of the node.
     */
    void update_local_transform();
};

} // namespace scene

} // namespace lmgl
