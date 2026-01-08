/*!
 * @file light.hpp
 * @brief Definition of the Light class and LightType enumeration.
 *
 * This file contains the declaration of the Light class, which represents
 * a light source in a 3D scene, as well as the LightType enumeration
 * that categorizes different types of lights.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */
#pragma once

#include <glm/glm.hpp>

namespace lmgl {

namespace scene {

/*!
 * @brief Enumeration of different light types.
 *
 * This enum defines the types of lights that can be used in the scene,
 * including directional lights, point lights, and spotlights.
 */
enum class LightType {
    Directional = 0, //!< Directional light source
    Point,           //!< Point light source
    Spot             //!< Spotlight source
};

/*!
 * @brief Class representing a light source in a 3D scene.
 *
 * The Light class encapsulates properties and behaviors of a light source,
 * including its type, color, intensity, and position/direction.
 */
class Light {
public:

    /*!
     * @brief Constructs a Light object with the specified type.
     *
     * @param type The type of the light (Directional, Point, Spot).
     */
    Light(LightType type = LightType::Point);

    /*!
     * @brief Virtual destructor for the Light class.
     *
     * Cleans up resources used by the Light object.
     */
    virtual ~Light() = default;

    /*!
     * @brief Gets the type of the light.
     *
     * @return The LightType of the light.
     */
    inline LightType get_type() const { return m_type; }

    /*!
     * @brief Sets the type of the light.
     *
     * @param type The new LightType for the light.
     */
    inline const glm::vec3& get_color() const { return m_color; }

    /*!
     * @brief Sets the color of the light.
     *
     * @param color The new color for the light as a glm::vec3.
     */
    inline void set_color(const glm::vec3& color) { m_color = color; }

    /*!
     * @brief Gets the intensity of the light.
     *
     * @return The intensity of the light as a float.
     */
    inline float get_intensity() const { return m_intensity; }

    /*!
     * @brief Sets the intensity of the light.
     *
     * @param intensity The new intensity for the light as a float.
     */
    inline void set_intesity(float intensity) { m_intensity = intensity; }

    /*!
     * @brief Gets the direction of the light.
     *
     * @return The direction of the light as a glm::vec3.
     */
    inline const glm::vec3& get_direction() const { return m_direction; }

    /*!
     * @brief Sets the direction of the light.
     *
     * @param direction The new direction for the light as a glm::vec3.
     */
    inline void set_direction(const glm::vec3& direction) { m_direction = glm::normalize(direction); }

    /*!
     * @brief Gets the position of the light.
     *
     * @return The position of the light as a glm::vec3.
     */
    inline const glm::vec3& get_position() const { return m_position; }

    /*!
     * @brief Sets the position of the light.
     *
     * @param position The new position for the light as a glm::vec3.
     */
    inline void set_position(const glm::vec3& position) { m_position = position; }

    /*!
     * @brief Gets the range of the light.
     *
     * @return The range of the light as a float.
     */
    inline float get_range() const { return m_range; }

    /*!
     * @brief Sets the range of the light.
     *
     * @param range The new range for the light as a float.
     */
    inline void set_range(float range) { m_range = range; }

    /*!
     * @brief Gets the inner cone angle of the spotlight.
     *
     * @return The inner cone angle in radians as a float.
     */
    inline float get_inner_cone() const { return m_inner_cone; }

    /*!
     * @brief Sets the inner cone angle of the spotlight.
     *
     * @param inner_cone The new inner cone angle in radians as a float.
     */
    inline void set_inner_cone(float inner_cone) { m_inner_cone = inner_cone; }

    /*!
     * @brief Gets the outer cone angle of the spotlight.
     *
     * @return The outer cone angle in radians as a float.
     */
    inline float get_outer_cone() const { return m_outer_cone; }

    /*!
     * @brief Sets the outer cone angle of the spotlight.
     *
     * @param outer_cone The new outer cone angle in radians as a float.
     */
    inline void set_outer_cone(float outer_cone) { m_outer_cone = outer_cone; }

    /*!
     * @brief Checks if the light casts shadows.
     *
     * @return True if the light casts shadows, false otherwise.
     */
    inline bool casts_shadows() const { return m_casts_shadows; }

    /*!
     * @brief Sets whether the light casts shadows.
     *
     * @param casts_shadows True to enable shadow casting, false to disable.
     */
    inline void set_casts_shadows(bool casts_shadows) { m_casts_shadows = casts_shadows; }

    // Factory methods
    
    /*! @brief Creates a directional light.
     *
     * @param direction The direction of the light as a glm::vec3.
     * @param color The color of the light as a glm::vec3 (default is white).
     * @return A shared pointer to the created Light object.
     */
    static std::shared_ptr<Light> create_directional(const glm::vec3& direction, const glm::vec3& color = glm::vec3(1.0f));

    /*! @brief Creates a point light.
     *
     * @param position The position of the light as a glm::vec3.
     * @param range The range of the light as a float (default is 10.0f).
     * @param color The color of the light as a glm::vec3 (default is white).
     * @return A shared pointer to the created Light object.
     */
    static std::shared_ptr<Light> create_point(const glm::vec3& position, 
                                        float range = 10.0f, 
                                        const glm::vec3& color = glm::vec3(1.0f));

    /*! @brief Creates a spotlight.
     *
     * @param position The position of the light as a glm::vec3.
     * @param direction The direction of the light as a glm::vec3.
     * @param range The range of the light as a float (default is 10.0f).
     * @param color The color of the light as a glm::vec3 (default is white).
     * @return A shared pointer to the created Light object.
     */
    static std::shared_ptr<Light> create_spot(const glm::vec3& position, 
                                       const glm::vec3& direction, float range = 10.0f,
                                       const glm::vec3& color = glm::vec3(1.0f));

protected:

    //! @brief The type of the light.
    LightType m_type;

    //! @brief The color of the light.
    glm::vec3 m_color{1.0f, 1.0f, 1.0f};

    //! @brief The intensity of the light.
    float m_intensity{1.0f};

    //! @brief The direction of the light.
    glm::vec3 m_direction{0.0f, -1.0f, 0.0f};

    //! @brief The position of the light.
    glm::vec3 m_position{0.0f, 0.0f, 0.0f};

    //! @brief The range of the light.
    float m_range{10.0f};

    //! @brief The inner cone angle of the spotlight in radians.
    float m_inner_cone{glm::radians(30.0f)};

    //! @brief The outer cone angle of the spotlight in radians.
    float m_outer_cone{glm::radians(45.0f)};

    //! @brief Flag indicating whether the light casts shadows.
    bool m_casts_shadows{false};
};

} // namespace scene

} // namespace lmgl
