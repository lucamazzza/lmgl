/*!
 * @file ovr_backend.hpp
 * @brief Optional LibOVR integration for stereoscopic camera generation.
 */

#pragma once

#include "lmgl/scene/camera.hpp"

#include <memory>

namespace lmgl {

namespace vr {

/*!
 * @brief Pair of eye cameras used for stereoscopic rendering.
 */
struct StereoCameraPair {
    std::shared_ptr<scene::Camera> left;
    std::shared_ptr<scene::Camera> right;
};

/*!
 * @brief Optional LibOVR backend.
 *
 * If LMGL is built with LibOVR support, this class can query runtime values
 * such as IPD. Otherwise, it gracefully falls back to a configurable default.
 */
class OvrBackend {
  public:
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
     * @brief Reports whether a LibOVR runtime session is available.
     *
     * @return True when connected to a LibOVR runtime.
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
