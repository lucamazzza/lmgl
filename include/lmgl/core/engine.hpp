/*!
 * @file engine.hpp
 * @author Luca Mazza
 * @brief Declaration of the Engine class for managing the main application loop using GLFW.
 *
 * This file contains the declaration of the Engine class, which is responsible for
 * initializing the GLFW window, running the main loop, and handling delta time calculations.
 * The Engine class is implemented as a singleton to ensure that only one instance
 * exists throughout the application lifecycle.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>

/*!
 * @namespace lmgl
 * @brief Namespace for the LMGL project.
 *
 * This namespace encapsulates all classes, functions, and variables related to
 * the LMGL project to avoid naming conflicts with other libraries or projects.
 */
namespace lmgl {

/*!
 * @namespace core
 * @brief Core components of the LMGL project.
 *
 * This namespace contains the essential classes and functions that form the
 * backbone of the LMGL project, including the Engine class for managing
 * the application loop.
 */
namespace core {

/*!
 * @class Engine
 * @bried A singleton class that manages the main application loop using GLFW.
 *
 * This class provides methods to initialize the engine, run the main loop,
 * and clean up resources. It also tracks the delta time between frames.
 * The engine is designed to be used in applications that require a rendering loop,
 * such as games or graphical simulations.
 *
 * @note This class is implemented as a singleton to ensure that only one instance
 * of the engine exists throughout the application lifecycle.
 */
class Engine {
public:

    /*!
     * @brief Get the singleton instance of the Engine.
     *
     * Maintains a static reference to the single Engine instance.
     * If the instance does not exist, it is created.
     *
     * @return Reference to the singleton Engine instance.
     */
    static Engine& get_instance();
    
    /*!
     * @brief Initialize the engine with the specified window dimensions and title.
     *
     * Initializes GLFW, creates a window, and sets up the necessary context for rendering.
     * Returns true if initialization is successful, false otherwise.
     *
     * @param w Width of the window.
     * @param h Height of the window.
     * @param title Title of the window.
     * @return True if initialization was successful, false otherwise.
     */
    bool init(int w, int h, std::string title);

    /*!
     * @brief Run the main application loop.
     *
     * The loop continues until the window is closed. The provided update callback
     * is called each frame with the delta time as an argument.
     *
     * @param update_callback A function to be called each frame with delta time.
     */
    void run(std::function<void(float)> update_callback);

    /*!
     * @brief Free resources and clean up the engine.
     *
     * Destroys the GLFW window and terminates GLFW.
     */
    void free();

    /*!
     * @brief Clear the screen.
     *
     * This method can be used to clear the screen before rendering a new frame.
     *
     * @param r Red component of the clear color (0.0 to 1.0).
     * @param g Green component of the clear color (0.0 to 1.0).
     * @param b Blue component of the clear color (0.0 to 1.0).
     * @param a Alpha component of the clear color (0.0 to 1.0), default is 1.0.
     */
    void clear(float r, float g, float b, float a = 1.0f);

    /*!
     * @brief Check if the engine is still running.
     *
     * The engine is considered running as long as the window has not been closed.
     *
     * @return True if the window is open, false otherwise.
     */
    bool is_running() const;

    /*!
     * @brief Get the GLFW window associated with the engine.
     *
     * The returned pointer can be used for further GLFW operations.
     *
     * @return Pointer to the GLFWwindow.
     */
    GLFWwindow* get_window() const;

    /*!
     * @brief Get the delta time between the current and last frame.
     *
     * This value is useful for frame-independent movement and animations.
     *
     * @return Delta time in seconds.
     */
    float get_delta_time() const;

private:

    //! @brief Private constructor to enforce singleton pattern.
    Engine() = default;

    //! @brief Default destructor.
    ~Engine() = default;

    //! @brief Delete copy constructor and assignment operator to prevent copies.
    Engine(const Engine&) = delete;

    //! @brief Delete assignment operator to prevent copies.
    Engine& operator=(const Engine&) = delete;

    /*!
     * @brief Callback function for framebuffer size changes.
     *
     * Adjusts the OpenGL viewport when the window is resized.
     *
     * @param window Pointer to the GLFW window.
     * @param w New width of the framebuffer.
     * @param h New height of the framebuffer.
     */
    static void fb_size_callback(GLFWwindow* window, int w, int h);

    //! @brief Pointer to the GLFW window.
    GLFWwindow* m_window = nullptr;

    //! @brief Delta time between frames.
    float m_delta_time = 0.0f;

    //! @brief Time of the last frame.
    float m_last_frame_time = 0.0f;
};

} // namespace core

} // namespace lmgl
