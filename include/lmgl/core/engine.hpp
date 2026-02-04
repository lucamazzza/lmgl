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
 * @brief Input key codes
 */
enum class Key {
    // Numbers
    Key0 = 48,
    Key1 = 49,
    Key2 = 50,
    Key3 = 51,
    Key4 = 52,
    Key5 = 53,
    Key6 = 54,
    Key7 = 55,
    Key8 = 56,
    Key9 = 57,
    // Letters
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    // Specials
    Space = 32,
    Esc = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    // Fn Keys
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    // Modifiers
    LShift = 340,
    LCtrl = 341,
    LAlt = 342,
    RShift = 344,
    RCtrl = 345,
    RAlt = 346
};

/*!
 * @brief Mouse button codes
 */
enum class MouseButton { Left = 0, Right = 1, Middle = 2 };

/*!
 * @brief Key/button states
 */
enum class InputState { Released = 0, Pressed = 1, Repeat = 2 };

/*!
 * @brief Cursor mode
 */
enum class CursorMode {
    Normal,  //!< Cursor is visible and behaves normally
    Hidden,  //!< Cursor is invisible but can still move freely
    Disabled //!< Cursor is locked to window center (FPS mode)
};

/*!
 * @brief VSync mode
 */
enum class VSyncMode {
    Off = 0,      //!< Disabled VSync - unlimited FPS
    On = 1,       //!< Standard VSync - Locks to monitor refresh rate
    Adaptive = -1 //!< Adaptive VSync - Only when FPS exceeds refresh rate
};

/*!
 * @brief A singleton class that manages the main application loop using GLFW.
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
    static Engine &get_instance();

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
    bool init(int w, int h, std::string title, bool resizable = true, bool vsync = true);

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
     * @brief Check if the engine is still running.
     *
     * The engine is considered running as long as the window has not been closed.
     *
     * @return True if the window is open, false otherwise.
     */
    bool is_running() const;

    /*!
     * @brief Shuts the engine down and closes the window.
     */
    void shutdown();

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
     * @brief Set VSync mode.
     *
     * @param mode The VSync mode to set
     */
    void set_vsync(VSyncMode mode);

    /*!
     * @brief Retrieves the window's width.
     *
     * @return the window width.
     */
    inline int get_width() const { return m_width; };

    /*!
     * @brief Retrieves the window's height.
     *
     * @return the window height.
     */
    inline int get_height() const { return m_height; };

    /*!
     * @brief Returns the aspect ratio of the window (width/height).
     *
     * @return the aspect ratio of the window.
     */
    float get_aspect_ratio() const;

    /*!
     * @brief Sets the window to a new size.
     *
     * @param width the new window width
     * @param height the new window height
     */
    void set_size(int width, int height);

    /*!
     * @brief Sets the window title to a new string.
     *
     * @param title the new title to set.
     */
    void set_title(const std::string &title);

    /*!
     * @brief Toggles fullscreen mode
     *
     * Fullscreen mode forces the window to render as fullscreen,
     * overriding system controls.
     *
     * @param fullscreen the new fullscreen state (true/false)
     */
    void set_fullscreen(bool fullscreen);

    /*!
     * @brief Checks if the window is in fullscreen mode.
     *
     * @return the fullsscreen state (true/false).
     */
    inline bool is_fullscreen() const { return m_fullscreen; };

    /*!
     * @brief Maximizes the window to occupy the entirety of the screen.
     */
    void maximize();

    /*!
     * @brief Minimizes the window.
     */
    void minimize();

    /*!
     * @brief Restores the window from minimized/maximized state.
     */
    void restore();

    /*!
     * @brief Checks if a key is currently pressed.
     *
     * @param key the key to check
     * @return whether the key is currently pressed or not.
     */
    bool is_key_pressed(Key key) const;

    /*!
     * @brief Checks if a key was just pressed this frame.
     *
     * @param key the key to check
     * @return whether the key was just pressed this frame or not.
     */
    bool is_key_just_pressed(Key key) const;

    /*!
     * @brief Checks if a key was just released this frame.
     *
     * @param key the key to check
     * @return whether the key was just released this frame or not.
     */
    bool is_key_just_released(Key key) const;

    /*!
     * @brief Checks if a mouse button is pressed.
     *
     * @param button the mouse button to check
     * @return whether the mouse button is pressed or not.
     */
    bool is_mouse_button_pressed(MouseButton button) const;

    /*!
     * @brief Retrieves the x position of the mouse in the window.
     *
     * @return The x position of the mouse in the window.
     */
    inline double get_mouse_x() const { return m_mouse_x; };

    /*!
     * @brief Retrieves the y position of the mouse in the window.
     *
     * @return The y position of the mouse in the window.
     */
    inline double get_mouse_y() const { return m_mouse_y; };

    /*!
     * @brief Retrieves the mouse position change (delta) between the last frame and the current frame.
     *
     * @param dx return parameter for the mouse position delta on the x axis
     * @param dy return parameter for the mouse position delta on the y axis
     */
    void get_mouse_delta(double &dx, double &dy) const;

    /*!
     * @brief Sets the mouse position to a (x,y) coordinate.
     *
     * @param x the new x coordinate component of the mouse position
     * @param y the new y coordinate component of the mouse position
     */
    void set_mouse_position(double x, double y);

    /*!
     * @brief Sets a new cursor mode to the engine.
     *
     * @param mode the new cursor mode to set
     */
    void set_cursor_mode(CursorMode mode);

    /*!
     * @brief Retrieves the offset of scroll on x and y axes since the last frame.
     *
     * @param xoffs the offset of scroll on the x axis since the last frame
     * @param yoffs the offset of scroll on the y axis since the last frame
     */
    void get_scroll_offset(double &xoffs, double &yoffs) const;

    /*!
     * @brief Get the GLFW window associated with the engine.
     *
     * The returned pointer can be used for further GLFW operations.
     *
     * @return Pointer to the GLFWwindow.
     */
    inline GLFWwindow *get_window() const { return m_window; };

    /*!
     * @brief Sets the callback responsible for window resizing.
     */
    inline void set_resize_callback(std::function<void(int, int)> callback) { m_resize_callback = callback; };

    /*!
     * @brief Get the delta time between the current and last frame.
     *
     * This value is useful for frame-independent movement and animations.
     *
     * @returnDelta time in seconds.
     */
    inline float get_delta_time() const { return m_delta_time; };

    /*!
     * @brief Retrieves the total time elapsed since the engine's start.
     *
     * @return The total time elapsed since the engine's start.
     */
    inline float get_time() const { return m_time; };

    /*!
     * @brief Retrieves the current FPS count.
     *
     * @return The current FPS count.
     */
    inline float get_fps() const { return m_fps; };

  private:
    //! @briefPrivate constructor to enforce singleton pattern.
    Engine() = default;

    //! @brief Default destructor.
    ~Engine() = default;

    //! @brief Delete copy constructor and assignment operator to prevent copies.
    Engine(const Engine &) = delete;

    //! @brief Delete assignment operator to prevent copies.
    Engine &operator=(const Engine &) = delete;

    /*!
     * @brief Callback function for framebuffer size changes.
     *
     * Adjusts the OpenGL viewport when the window is resized.
     *
     * @param window Pointer to the GLFW window.
     * @param w New width of the framebuffer.
     * @param h New height of the framebuffer.
     */
    static void fb_size_callback(GLFWwindow *window, int w, int h);

    /*!
     * @brief Callback function for the key states interaction (keyboard).
     *
     * Executes custom behaviors when keys are pressed/released.
     * Fundamental for user interactability.
     *
     * @param window Pointer to the GLFW window.
     * @param key reference key on which to bind a behavior.
     * @param scancode platform-specific unique identifier for the key.
     * @param action event (press/repeat/release) to bind a behavior to.
     * @param mods modifiers for the key action (Shift+/Alt+/Ctrl+)
     */
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    /*!
     * @brief Callback function for mouse buttons interaction
     *
     * Executes custom behaviors when mouse buttons are pressed/released.
     * Fundamental for mouse interactabilty.
     *
     * @param window Pointer to the GLFW window.
     * @param button the mouse button on which to bind a behavior.
     * @param action event (press/release) to bind a behavior to.
     * @param mods modifiers for the mouse button action (Shift+/Alt+/Ctrl+)
     */
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    /*!
     * @brief Callback function for mouse cursor position interaction.
     *
     * Executes custom behaviors when the mouse cursor is in a specified position.
     *
     * @param window Pointer to the GLFW window.
     * @param xpos x axis position to bind the cursor behavior to.
     * @param ypos y axis position to bind the cursor behavior to.
     */
    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

    /*!
     * @brief Callback function for scrolling interaction.
     *
     * Executes custom behaviors when scrolling on the window.
     *
     * @param window Pointer to the GLFW window.
     * @param xoffs offset on the x axis of the scroll action.
     * @param yoffs offset on the y axis of the scroll action.
     */
    static void scroll_callback(GLFWwindow *window, double xoffs, double yoffs);

    /*!
     * @brief Updates the input state resetting "just pressed/released" states.
     */
    void update_input_state();

    //! @brief Pointer to the GLFW window.
    GLFWwindow *m_window = nullptr;

    //! @brief Current window width.
    int m_width = 0;

    //! @brief Current window height.
    int m_height = 0;

    //! @brief Fullscreen mode flag.
    bool m_fullscreen = false;

    //! @brief Delta time between frames.
    float m_delta_time = 0.0f;

    //! @brief Time of the last frame.
    float m_last_frame_time = 0.0f;

    //! @brief Time elapsed since the engine's start.
    float m_time = 0.0f;

    //! @brief Current FPS count.
    float m_fps = 0.0f;

    //! @brief Time local count for FPS.
    int m_frame_count = 0;

    //! @brief Time elapsed since the last FPS update.
    float m_fps_timer = 0.0f;

    //! @brief Key input states.
    bool m_key_states[512] = {false};

    //! @brief "Last frame just pressed" key input states.
    bool m_key_just_pressed[512] = {false};

    //! @brief "Last frame just released" key input states.
    bool m_key_just_released[512] = {false};

    //! @brief Mouse button input states.
    bool m_mouse_button_states[8] = {false};

    //! @brief Current x axis position of the mouse on the window.
    double m_mouse_x = 0.0;

    //! @brief Current y axis position of the mouse on the window.
    double m_mouse_y = 0.0;

    //! @brief Last frame's x axis position of the mouse on the window.
    double m_last_mouse_x = 0.0;

    //! @brief Last frame's y axis position of the mouse on the window.
    double m_last_mouse_y = 0.0;

    //! @brief Amount of x axis scroll on the window.
    double m_scroll_x = 0.0;

    //! @brief Amount of y axis scroll on the window.
    double m_scroll_y = 0.0;

    //! @brief Resize callback function reference.
    std::function<void(int, int)> m_resize_callback;
};

} // namespace core

} // namespace lmgl
