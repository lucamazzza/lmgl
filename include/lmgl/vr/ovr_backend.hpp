/*!
 * @file ovr_backend.hpp
 * @brief Optional OpenVR integration for stereoscopic camera generation.
 */

#pragma once

#include "lmgl/scene/camera.hpp"

#include <memory>

namespace lmgl {

/*!
 * @namespace lmgl::vr
 * @brief Features relative to the VR functionalities
 */
namespace vr {

/*!
 * @brief Pair of eye cameras used for stereoscopic rendering.
 */
struct StereoCameraPair {
    //! Camera configured for the left eye.
    std::shared_ptr<scene::Camera> left;
    //! Camera configured for the right eye.
    std::shared_ptr<scene::Camera> right;
};

/*!
 * @brief Eye selector used for stereo rendering operations.
 */
enum class OvrEye { Left = 0, Right = 1 };

/*!
 * @brief Optional OpenVR backend.
 *
 * If LMGL is built with OpenVR support, this class can query runtime values
 * such as IPD. Otherwise, it gracefully falls back to a configurable default.
 */
class OvrBackend {
  public:
    OvrBackend();
    ~OvrBackend();
    /*!
     * @brief Initializes the backend.
     *
     * @return True if initialization succeeded.
     */
    bool initialize();

    /*!
     * @brief Shuts down the backend and releases runtime resources.
     */
    void shutdown();

    /*!
     * @brief Reports whether the backend has been initialized.
     *
     * @return True when initialize() completed successfully.
     */
    bool is_initialized() const;

    /*!
     * @brief Reports whether an OpenVR runtime session is available.
     *
     * @return True when connected to an OpenVR runtime.
     */
    bool has_runtime() const;

    /*!
     * @brief Returns the inter-pupillary distance used for stereo cameras.
     *
     * @return IPD in meters.
     */
    float get_ipd() const;

    /*!
     * @brief Sets a fallback IPD to use when runtime data is unavailable.
     *
     * @param ipd Inter-pupillary distance in meters.
     */
    void set_fallback_ipd(float ipd);

    /*!
     * @brief Builds left/right eye cameras from a center camera.
     *
     * @param center_camera Camera that defines shared projection/orientation.
     * @return Stereo camera pair.
     */
    StereoCameraPair build_stereo_cameras(const scene::Camera &center_camera);

    /*!
     * @brief Submit one rendered eye texture to OpenVR compositor.
     *
     * @param eye Eye selector.
     * @param texture_id OpenGL texture id.
     * @return True if submission succeeded.
     */
    bool pass(OvrEye eye, unsigned int texture_id);

    /*!
     * @brief Finalize a compositor frame after both eyes are submitted.
     */
    void render();

  private:
    bool m_initialized = false;
    bool m_has_runtime = false;
    float m_ipd = 0.064f;

#if defined(LMGL_VR_OVR)
    struct RuntimeState;
    std::unique_ptr<RuntimeState> m_runtime;
#endif
};

} // namespace vr

} // namespace lmgl
