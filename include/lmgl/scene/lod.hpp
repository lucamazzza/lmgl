/*!
 * @file lod.hpp
 * @brief Level of Detail (LOD) management for 3D objects.
 *
 * This file defines the LOD class, which manages multiple levels of detail for 3D objects.
 * It allows for efficient rendering by selecting the appropriate mesh based on the distance
 * from the camera. The LOD class supports adding LOD levels and retrieving the correct mesh
 * based on distance calculations.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/scene/mesh.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace lmgl {

namespace scene {

/*!
 * @brief Level of Detail (LOD) structure representing a single LOD level.
 *
 * Each LOD level contains a mesh and the maximum distance squared at which this LOD is used.
 * The distance is squared to avoid unnecessary square root calculations during distance comparisons.
 * This structure is used within the LOD class to manage multiple levels of detail for 3D objects.
 *
 * @see LOD
 */
struct LODLevel {
    //! @brief Shared pointer to the mesh representing this LOD level.
    std::shared_ptr<Mesh> mesh;

    //! @brief Maximum distance squared at which this LOD level is used.
    float max_distance_sq;

    /*!
     * @brief Constructor for LODLevel.
     *
     * @param m Shared pointer to the mesh for this LOD level.
     * @param distance Maximum distance at which this LOD level is used.
     */
    LODLevel(std::shared_ptr<Mesh> m, float distance) : mesh(m), max_distance_sq(distance * distance) {}
};

/*!
 * @brief Level of Detail (LOD) management class.
 *
 * This class manages multiple levels of detail for 3D objects, allowing for efficient rendering
 * based on the distance from the camera. It provides methods to add LOD levels and retrieve
 * the appropriate mesh based on distance.
 */
class LOD {
  public:
    //! @brief Default constructor.
    LOD() = default;

    //! @brief Default destructor.
    ~LOD() = default;

    /*!
     * @brief Adds a new LOD level.
     *
     * The levels should be added in order of increasing maximum distance.
     *
     * @param mesh Shared pointer to the mesh for the new LOD level.
     * @param max_distance Maximum distance at which this LOD level is used.
     */
    void add_level(std::shared_ptr<Mesh> mesh, float max_distance);

    /*!
     * @brief Retrieves the appropriate mesh based on the squared distance.
     *
     * The method iterates through the LOD levels and returns the mesh corresponding
     * to the first level where the squared distance is less than or equal to the
     * maximum distance squared for that level. If no such level is found, it returns
     * the mesh of the highest LOD level.
     *
     * @param distance_sq Squared distance from the camera to the object.
     * @return Shared pointer to the mesh corresponding to the appropriate LOD level.
     */
    std::shared_ptr<Mesh> get_mesh(float distance_sq) const;

    /*!
     * @brief Retrieves the appropriate mesh based on camera and object positions.
     *
     * This method calculates the squared distance between the camera and the object,
     * then calls the other get_mesh method to retrieve the appropriate mesh.
     *
     * @param camera_pos Position of the camera in 3D space.
     * @param object_pos Position of the object in 3D space.
     * @return Shared pointer to the mesh corresponding to the appropriate LOD level.
     */
    std::shared_ptr<Mesh> get_mesh(const glm::vec3 &camera_pos, const glm::vec3 &object_pos) const;

    /*!
     * @brief Retrieves the number of LOD levels.
     *
     * @return Number of LOD levels.
     */
    inline size_t get_level_count() const { return m_levels.size(); }

    /*!
     * @brief Retrieves a specific LOD level by index.
     *
     *
     * @param index Index of the LOD level to retrieve.
     * @return Reference to the LODLevel at the specified index.
     */
    inline const LODLevel &get_level(size_t index) const { return m_levels[index]; }

    /*!
     * @brief Checks if there are any LOD levels defined.
     *
     * @return True if there are LOD levels, false otherwise.
     */
    inline bool has_levels() const { return !m_levels.empty(); }

    /*!
     * @brief Clears all LOD levels.
     */
    inline void clear() { m_levels.clear(); }

  private:

    //! @brief Vector of LOD levels.
    std::vector<LODLevel> m_levels;
};

} // namespace scene

} // namespace lmgl
