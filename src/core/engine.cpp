#include "lmgl/core/engine.hpp"
#include "GLFW/glfw3.h"

#include <iostream>

namespace lmgl {

namespace core {

Engine &Engine::get_instance() {
    static Engine instance;
    return instance;
}

bool Engine::init(int w, int h, std::string title, bool resizable, bool vsync) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    m_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }
    m_width = w;
    m_height = h;
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, fb_size_callback);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCursorPosCallback(m_window, cursor_position_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    set_vsync(vsync ? VSyncMode::On : VSyncMode::Off);
    m_last_frame_time = static_cast<float>(glfwGetTime());
    return true;
}

void Engine::run(std::function<void(float)> update_callback) {
    while (!glfwWindowShouldClose(m_window)) {
        float current_time = static_cast<float>(glfwGetTime());
        m_delta_time = current_time - m_last_frame_time;
        m_last_frame_time = current_time;
        m_time = current_time;

        m_fps_timer += m_delta_time;
        m_frame_count++;
        if (m_fps_timer >= 1.0f) {
            m_fps = m_frame_count / m_fps_timer;
            m_frame_count = 0;
            m_fps_timer = 0.0f;
        }
        update_input_state();
        update_callback(m_delta_time);
        glfwSwapBuffers(m_window);
        glfwPollEvents();
        m_scroll_x = 0.0f;
        m_scroll_y = 0.0f;
    }
}

void Engine::free() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Engine::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Engine::is_running() const { return m_window && !glfwWindowShouldClose(m_window); }

void Engine::shutdown() {
    if (m_window) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

float Engine::get_aspect_ratio() const {
    return m_height > 0 ? static_cast<float>(m_width) / static_cast<float>(m_height) : 1.0f;
}
void Engine::set_size(int width, int height) {
    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
        m_width = width;
        m_height = height;
    }
}

void Engine::set_title(const std::string &title) {
    if (m_window) {
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

void Engine::set_fullscreen(bool fullscreen) {
    if (m_fullscreen == fullscreen)
        return;

    m_fullscreen = fullscreen;
    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(m_window, nullptr, 100, 100, m_width, m_height, 0);
    }
}

void Engine::maximize() {
    if (m_window) {
        glfwMaximizeWindow(m_window);
    }
}

void Engine::minimize() {
    if (m_window) {
        glfwIconifyWindow(m_window);
    }
}

void Engine::restore() {
    if (m_window) {
        glfwRestoreWindow(m_window);
    }
}

void Engine::set_vsync(VSyncMode mode) { glfwSwapInterval(static_cast<int>(mode)); }

bool Engine::is_key_pressed(Key key) const {
    int k = static_cast<int>(key);
    return k >= 0 && k < 512 ? m_key_states[k] : false;
}

bool Engine::is_key_just_pressed(Key key) const {
    int k = static_cast<int>(key);
    return k >= 0 && k < 512 ? m_key_just_pressed[k] : false;
}

bool Engine::is_key_just_released(Key key) const {
    int k = static_cast<int>(key);
    return k >= 0 && k < 512 ? m_key_just_released[k] : false;
}

bool Engine::is_mouse_button_pressed(MouseButton button) const {
    int b = static_cast<int>(button);
    return b >= 0 && b < 8 ? m_mouse_button_states[b] : false;
}

void Engine::get_mouse_delta(double &dx, double &dy) const {
    dx = m_mouse_x - m_last_mouse_x;
    dy = m_mouse_y - m_last_mouse_y;
}

void Engine::set_mouse_position(double x, double y) {
    if (m_window) {
        glfwSetCursorPos(m_window, x, y);
    }
}

void Engine::set_cursor_mode(CursorMode mode) {
    if (!m_window)
        return;
    switch (mode) {
    case CursorMode::Normal:
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case CursorMode::Hidden:
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
    case CursorMode::Disabled:
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
}

void Engine::get_scroll_offset(double &xoffset, double &yoffset) const {
    xoffset = m_scroll_x;
    yoffset = m_scroll_y;
}

void Engine::fb_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->m_width = width;
        engine->m_height = height;
        if (engine->m_resize_callback) {
            engine->m_resize_callback(width, height);
        }
    }
}

void Engine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine || key < 0 || key >= 512)
        return;

    if (action == GLFW_PRESS) {
        engine->m_key_states[key] = true;
        engine->m_key_just_pressed[key] = true;
    } else if (action == GLFW_RELEASE) {
        engine->m_key_states[key] = false;
        engine->m_key_just_released[key] = true;
    }
}

void Engine::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine || button < 0 || button >= 8)
        return;

    engine->m_mouse_button_states[button] = (action == GLFW_PRESS);
}

void Engine::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine)
        return;

    engine->m_last_mouse_x = engine->m_mouse_x;
    engine->m_last_mouse_y = engine->m_mouse_y;
    engine->m_mouse_x = xpos;
    engine->m_mouse_y = ypos;
}

void Engine::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine)
        return;

    engine->m_scroll_x = xoffset;
    engine->m_scroll_y = yoffset;
}

void Engine::update_input_state() {
    // Reset "just pressed/released" states
    for (int i = 0; i < 512; ++i) {
        m_key_just_pressed[i] = false;
        m_key_just_released[i] = false;
    }
}

} // namespace core

} // namespace lmgl
