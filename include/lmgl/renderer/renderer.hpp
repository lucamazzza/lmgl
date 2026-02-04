/*!
 * @file renderer.hpp
 * @brief Declares classes and structures for rendering scenes using OpenGL.
 *
 * This header file contains the definitions for the renderer namespace, the
 * Renderer class, and related enumerations such as RenderMode and RenderLayer.
 * These types coordinate scene rendering, camera setup, and drawing operations
 * in an OpenGL-based rendering pipeline.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/framebuffer.hpp"
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"

#include <memory>
#include <vector>

namespace lmgl {

/*!
 * @brief Namespace for rendering-related classes and functions.
 *
 * This namespace contains classes and functions that are responsible for
 * rendering scenes, managing shaders, and handling graphical output.
 */
namespace renderer {

/*!
 * @brief Enumerates the different rendering modes.
 *
 * This enumeration defines the various modes in which objects can be rendered,
 * such as solid, wireframe, and points. Each mode affects how the geometry
 * is displayed on the screen.
 */
enum class RenderMode { Solid = 0, Wireframe, Points };

/*!
 * @brief Enumerates the different render layers.
 *
 * This enumeration defines layers for rendering order, such as skybox,
 * opaque objects, transparent objects, and UI elements. Each layer has
 * a specific priority to ensure correct rendering order.
 */
enum class RenderLayer { Skybox = 0, Opaque = 100, Transparent = 200, UI = 300 };

/*!
 * @brief Manages the rendering of scenes.
 *
 * This class is responsible for rendering 3D scenes using OpenGL.
 * It handles the rendering pipeline, including setting up the camera,
 * managing render modes, and optimizing draw calls.
 */
class Renderer {
  public:
    /*!
     * @brief Constructor for the Renderer class.
     *
     * Initializes the renderer with default settings.
     */
    Renderer();

    /*!
     * @brief Destructor for the Renderer class.
     *
     * Cleans up any resources used by the renderer.
     */
    ~Renderer() = default;

    /*!
     * @brief Render a scene from the perspective of a camera.
     *
     * This method takes a scene and a camera as input and renders the scene
     * using the specified camera's view and projection matrices. It also
     * applies the current render mode and handles depth testing and blending
     * based on the renderer's settings.
     *
     * @param scene Shared pointer to the scene to be rendered.
     * @param camera Shared pointer to the camera used for rendering.
     */
    void render(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> camera);

    /*!
     * @brief Set the rendering mode.
     *
     * This method allows changing the rendering mode of the renderer.
     * The mode can be set to solid, wireframe, or points, affecting
     * how the geometry is displayed.
     *
     * @param mode The desired rendering mode.
     */
    void set_render_mode(RenderMode mode);

    /*!
     * @brief Enable or disable face culling.
     *
     * This method allows toggling face culling, which can improve
     * performance by not rendering faces of geometry that are not visible
     * to the camera.
     *
     * @param enabled True to enable culling, false to disable.
     */
    void set_culling(bool enabled);

    /*!
     * @brief Enable or disable depth testing.
     *
     * This method allows toggling depth testing, which ensures that
     * closer objects are rendered in front of farther ones.
     *
     * @param enabled True to enable depth testing, false to disable.
     */
    void set_depth_test(bool enabled);

    /*!
     * @brief Enable or disable blending.
     *
     * This method allows toggling blending, which is used for rendering
     * transparent objects.
     *
     * @param enabled True to enable blending, false to disable.
     */
    void set_blending(bool enabled);

    /*!
     * @brief Get the current rendering mode.
     *
     * This method returns the current rendering mode of the renderer.
     *
     * @return The current RenderMode.
     */
    inline RenderMode get_render_mode() const { return m_render_mode; }

    /*!
     * @brief Get the number of draw calls made in the last render.
     *
     * This method returns the number of draw calls that were made
     * during the last rendering operation.
     *
     * @return Number of draw calls.
     */
    inline unsigned int get_draw_calls() const { return m_draw_calls; }

    /*!
     * @brief Get the number of triangles rendered in the last render.
     *
     * This method returns the total number of triangles that were
     * rendered during the last rendering operation.
     *
     * @return Number of triangles rendered.
     */
    inline unsigned int get_triangles_count() const { return m_triangles_count; }

    /*!
     * @brief Resizes the framebuffer to specific width and height.
     *
     * @param width The width to resize to.
     * @param height The height to resize to.
     */
    void resize(int width, int height);

    /*!
     * @brief Sets the tone map mode.
     *
     * The tone map modes are:
     *     - 0 = none;
     *     - 1 = reinhard;
     *     - 2 = aces.
     *
     * @param mode The tone map mode.
     */
    inline void set_tone_map_mode(int mode) { m_tone_map_mode = mode; }

    /*!
     * @brief Sets the exposure of the tone map.
     *
     * @param exposure The exposure float value.
     */
    inline void set_exposure(float exposure) { m_exposure = exposure; }

    /*!
     * @brief Sets the gamma of the tone map.
     *
     * @param gamma The gamma float value.
     */
    inline void set_gamma(float gamma) { m_gamma = gamma; }

    /*!
     * @brief Enables post processing effects based on tone map mode and exposure/gamma.
     *
     * @param enabled Whether the postprocessing effects are enabled.
     */
    void enable_post_processing(bool enabled);

  private:
    //! @brief Current rendering mode.
    RenderMode m_render_mode;

    //! @brief Number of draw calls made in the last render.
    unsigned int m_draw_calls;

    //! @brief Number of triangles rendered in the last render.
    unsigned int m_triangles_count;

    //! @brief Flags for depth testing, face culling, and blending.
    bool m_depth_test_enabled;

    //! @brief Flags for depth testing, face culling, and blending.
    bool m_culling_enabled;

    //! @brief Flags for depth testing, face culling, and blending.
    bool m_blending_enabled;

    /*!
     * @brief Structure representing an item to be rendered.
     *
     * This structure holds a mesh, its transformation matrix,
     * and its distance to the camera for sorting purposes.
     */
    struct RenderItem {

        //! @brief Shared pointer to the mesh to be rendered.
        std::shared_ptr<scene::Mesh> mesh;

        //! @brief Transformation matrix for the mesh.
        glm::mat4 transform;

        //! @brief Distance from the mesh to the camera.
        float distance_to_camera;

        //! @brief Render layer of the mesh.
        RenderLayer layer;

        //! @brief Flag indicating if the mesh is transparent.
        bool is_transparent;
    };

    //! @brief Render queue containing items to be rendered.
    std::vector<RenderItem> m_render_queue;

    /*!
     * @brief Build the render queue from the scene graph.
     *
     * This method traverses the scene graph starting from the given node,
     * applying transformations and collecting renderable meshes into
     * the output render queue. It calculates the distance to the camera
     * for each mesh to facilitate sorting.
     *
     * @param node Shared pointer to the current scene node.
     * @param camera Shared pointer to the camera used for distance calculations.
     * @param par_transform Parent transformation matrix.
     * @param out_items Vector to store the collected render items.
     */
    void build_render_queue(std::shared_ptr<scene::Node> node, std::shared_ptr<scene::Camera> camera,
                            const glm::mat4 &par_transform, std::vector<RenderItem> &out_items);

    /*!
     * @brief Build the render queue with frustum culling.
     *
     * This method traverses the scene graph starting from the given node,
     * applying transformations and collecting renderable meshes into
     * the output render queue. It performs frustum culling to exclude
     * meshes that are outside the camera's view frustum.
     *
     * @param node Shared pointer to the current scene node.
     * @param camera Shared pointer to the camera used for distance calculations.
     * @param par_transform Parent transformation matrix.
     * @param out_items Vector to store the collected render items.
     * @param frustum The view frustum used for culling.
     */
    void build_render_queue_culled(std::shared_ptr<scene::Node> node, std::shared_ptr<scene::Camera> camera,
                                   const glm::mat4 &par_transform, std::vector<RenderItem> &out_items,
                                   const scene::Frustum &frustum);

    /*!
     * @brief Sort the render queue based on distance to the camera.
     *
     * This method sorts the render items in the render queue to optimize
     * rendering order. Opaque objects are sorted front-to-back, while
     * transparent objects are sorted back-to-front.
     */
    void sort_render_queue(std::vector<RenderItem> &items);

    /*!
     * @brief Render a single mesh with the given transformation and camera.
     *
     * This method handles the actual rendering of a mesh, applying
     * the provided transformation matrix and using the camera's
     * view and projection matrices.
     *
     * @param mesh Shared pointer to the mesh to be rendered.
     * @param transform Transformation matrix to be applied to the mesh.
     * @param camera Shared pointer to the camera used for rendering.
     */
    void render_mesh(std::shared_ptr<scene::Mesh> mesh, const glm::mat4 &transform,
                     std::shared_ptr<scene::Camera> camera);

    /*!
     * @brief Apply the current render mode settings.
     *
     * This method configures OpenGL state based on the current
     * render mode, adjusting polygon mode and other relevant settings.
     */
    void apply_render_mode();

    /*!
     * @brief Collects all the lights and puts them in type-specific containers
     *
     * @param scene The scene to collect lights from.
     */
    void collect_lights(std::shared_ptr<scene::Scene> scene);

    /*!
     * @brief Collects lights associated with a node and its children, and puts them into type-specific containers
     *
     * @param the node to collect lights from
     */
    void collect_node_lights(std::shared_ptr<scene::Node> node);

    /*!
     * @brief Binds all the lights with a shader.
     *
     * @param shader The shader to use.
     */
    void bind_lights(std::shared_ptr<renderer::Shader> shader);

    /*!
     * @brief Binds a material if it differs from the last bound material.
     *
     * This method implements material caching to avoid redundant state changes.
     * It only binds the material if it is different from the previously bound one.
     *
     * @param material The material to bind.
     * @param shader The shader to use.
     */
    void bind_material(std::shared_ptr<scene::Material> material, std::shared_ptr<renderer::Shader> shader);

    /*!
     * @brief Clears the material cache.
     *
     * This method resets the cached material state, forcing the next
     * material binding to occur regardless of the previous state.
     */
    void clear_material_cache();

  private:
    //! Directional lights to render.
    std::vector<std::shared_ptr<scene::Light>> m_directional_lights;

    //! Point lights to render
    std::vector<std::shared_ptr<scene::Light>> m_point_lights;

    //! Spot lights to render
    std::vector<std::shared_ptr<scene::Light>> m_spot_lights;

    //! Default material for meshes without materials
    std::shared_ptr<scene::Material> m_default_material;

    //! Cached material to minimize state changes
    std::shared_ptr<scene::Material> m_last_bound_material;

    //! Framebuffer for postprocess effects
    std::unique_ptr<Framebuffer> m_framebuffer;

    //! Shader for postprocess effects
    std::shared_ptr<Shader> m_postprocess_shader;

    //! Screen quad mesh
    std::shared_ptr<scene::Mesh> m_screen_quad;

    //! Tone map mode (0 = none, 1 = reinhard, 2 = aces [default])
    int m_tone_map_mode = 2;

    //! Tonemap exposure
    float m_exposure = 1.0f;

    //! Tonemap gamma
    float m_gamma = 2.2f;

    //! Window width and height for viewport
    int m_window_width = 1280;
    int m_window_height = 720;
};

} // namespace renderer

} // namespace lmgl
