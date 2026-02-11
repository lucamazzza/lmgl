/*!
 * @file ui_loader.hpp
 * @brief JSON-based UI loader for declarative UI creation.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/ui/canvas.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace lmgl {

namespace ui {

/*!
 * @brief Callback registry for UI event handlers.
 */
class UICallbackRegistry {
  public:
    /*!
     * @brief Register a callback function.
     *
     * @param name Callback name (used in JSON).
     * @param callback Function to call.
     */
    void register_callback(const std::string &name, std::function<void()> callback);

    /*!
     * @brief Get a registered callback.
     *
     * @param name Callback name.
     * @return Callback function, or nullptr if not found.
     */
    std::function<void()> get_callback(const std::string &name) const;

    /*!
     * @brief Check if callback exists.
     *
     * @param name Callback name.
     * @return True if registered.
     */
    bool has_callback(const std::string &name) const;

  private:
    std::map<std::string, std::function<void()>> m_callbacks;
};

/*!
 * @brief Result of UI loading operation.
 */
struct UILoadResult {
    //! @brief Whether loading succeeded
    bool success{false};

    //! @brief Error message if loading failed
    std::string error;

    //! @brief Loaded canvas
    std::shared_ptr<Canvas> canvas;

    //! @brief Map of element names to elements
    std::map<std::string, std::shared_ptr<UIElement>> elements;
};

/*!
 * @brief JSON-based UI loader.
 *
 * Loads UI definitions from JSON files and creates UI elements.
 */
class UILoader {
  public:
    /*!
     * @brief Constructor.
     */
    UILoader();

    /*!
     * @brief Load UI from JSON file.
     *
     * @param filepath Path to JSON file.
     * @param callbacks Callback registry for event handlers.
     * @return Load result with canvas and elements.
     */
    UILoadResult load_from_file(const std::string &filepath, const UICallbackRegistry &callbacks);

    /*!
     * @brief Load UI from JSON string.
     *
     * @param json_str JSON string.
     * @param callbacks Callback registry for event handlers.
     * @return Load result with canvas and elements.
     */
    UILoadResult load_from_string(const std::string &json_str, const UICallbackRegistry &callbacks);

    /*!
     * @brief Set base path for resolving relative paths (fonts, images).
     *
     * @param base_path Base directory path.
     */
    void set_base_path(const std::string &base_path);

  private:
    //! @brief Base path for resource loading
    std::string m_base_path;

    /*!
     * @brief Internal load implementation.
     *
     * @param json_data JSON data object.
     * @param callbacks Callback registry.
     * @return Load result.
     */
    UILoadResult load_internal(const void *json_data, const UICallbackRegistry &callbacks);
};

} // namespace ui

} // namespace lmgl
