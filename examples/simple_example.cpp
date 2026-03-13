/*!
 * @file simple_example.cpp
 * @brief Minimal example showcasing the improved LMGL API.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include <iostream>
#include <lmgl/lmgl.hpp>

int main() {
    using namespace lmgl;

    // Initialize engine with new convenience API
    auto &engine = Engine::get_instance();
    if (!engine.init(1280, 720, "Simple LMGL Example")) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    std::cout << "LMGL Engine initialized!" << std::endl;
    std::cout << "   Window: " << engine.get_width() << "x" << engine.get_height() << std::endl;
    std::cout << "   Aspect: " << engine.get_aspect_ratio() << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "   ESC - Exit" << std::endl;
    std::cout << "   SPACE - Change color" << std::endl;

    // Simple UI showing FPS and info
    auto canvas = std::make_shared<ui::Canvas>(engine.get_width(), engine.get_height());

    auto fps_text = std::make_shared<ui::Text>("FPS: 0", "fps");
    fps_text->set_position(glm::vec2(10.0f, 10.0f));
    fps_text->set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    canvas->add_element(fps_text);

    auto info_text = std::make_shared<ui::Text>("Press SPACE to change color", "info");
    info_text->set_position(glm::vec2(10.0f, 40.0f));
    info_text->set_color(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    canvas->add_element(info_text);

    auto input_text = std::make_shared<ui::Text>("Using static Input class!", "input_info");
    input_text->set_position(glm::vec2(10.0f, 70.0f));
    input_text->set_color(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    canvas->add_element(input_text);

    std::cout << "\nRunning main loop...\n" << std::endl;

    // Background color
    float hue = 0.0f;

    // Main loop using the improved Input API
    engine.run([&](float dt) {
        // Handle input using static Input class (no engine reference needed!)
        if (Input::is_key_pressed(Key::Esc)) {
            engine.shutdown();
        }

        // Change color with space
        if (Input::is_key_just_pressed(Key::Space)) {
            hue += 0.1f;
            if (hue > 1.0f)
                hue = 0.0f;
        }

        // Update FPS text
        fps_text->set_text("FPS: " + std::to_string(static_cast<int>(engine.get_fps())));

        // Resize canvas if window size changed
        int width, height;
        glfwGetFramebufferSize(engine.get_window(), &width, &height);
        canvas->resize(width, height);

        // Animated background color
        float r = std::sin(hue * 6.28f) * 0.5f + 0.5f;
        float g = std::sin(hue * 6.28f + 2.09f) * 0.5f + 0.5f;
        float b = std::sin(hue * 6.28f + 4.18f) * 0.5f + 0.5f;

        // Clear with animated color
        glClearColor(r * 0.3f, g * 0.3f, b * 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render UI
        canvas->render();
    });

    std::cout << "\nShutting down..." << std::endl;
    engine.free();

    return 0;
}
