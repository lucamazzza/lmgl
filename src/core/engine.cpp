#include "lmgl/core/engine.hpp"

#include <iostream>

namespace lmgl {

namespace core {

Engine& Engine::get_instance() {
    static Engine instance;
    return instance;
}

bool Engine::init(int w, int h, std::string title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl; 
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    m_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, fb_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }
    int fbw, fbh;
    glfwGetFramebufferSize(m_window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    return true;
}

void Engine::run(std::function<void(float)> update_callback) {
    m_last_frame_time = static_cast<float>(glfwGetTime());
    while(!glfwWindowShouldClose(m_window)) {
        float current_time = static_cast<float>(glfwGetTime());
        m_delta_time = current_time - m_last_frame_time;
        m_last_frame_time = current_time;
        glfwPollEvents();
        if (update_callback) update_callback(m_delta_time);
        glfwSwapBuffers(m_window);
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

bool Engine::is_running() const {
    return m_window && !glfwWindowShouldClose(m_window);
}

GLFWwindow* Engine::get_window() const {
    return m_window;
}

float Engine::get_delta_time() const {
    return m_delta_time;
}

void Engine::fb_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

} // namespace core

} // namespace lmgl
