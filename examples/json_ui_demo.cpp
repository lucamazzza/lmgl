/*!
 * @file json_ui_demo.cpp
 * @brief Demo showcasing JSON-based UI loading.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/core/engine.hpp"
#include "lmgl/ui/button.hpp"
#include "lmgl/ui/slider.hpp"
#include "lmgl/ui/toggle.hpp"
#include "lmgl/ui/ui_loader.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

int main() {
    using namespace lmgl;

    auto &engine = core::Engine::get_instance();

    if (!engine.init(1280, 720, "JSON UI Demo")) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    // Create callback registry
    ui::UICallbackRegistry callbacks;

    // Register button callbacks
    callbacks.register_callback("onButton1Click", []() { std::cout << "Button 1 clicked from JSON!" << std::endl; });

    callbacks.register_callback("onButton2Click",
                                []() { std::cout << "Reset button clicked from JSON!" << std::endl; });

    // Load UI from JSON
    ui::UILoader loader;
    loader.set_base_path("examples/assets");

    auto result = loader.load_from_file("examples/assets/ui_demo.json", callbacks);

    if (!result.success) {
        std::cerr << "Failed to load UI: " << result.error << std::endl;
        engine.free();
        return -1;
    }

    std::cout << "✅ UI loaded from JSON successfully!" << std::endl;
    std::cout << "Loaded " << result.elements.size() << " named elements" << std::endl;

    // Get references to specific elements
    auto button1 = std::dynamic_pointer_cast<ui::Button>(result.elements["button1"]);
    auto button2 = std::dynamic_pointer_cast<ui::Button>(result.elements["button2"]);
    auto toggle1 = std::dynamic_pointer_cast<ui::Toggle>(result.elements["toggle1"]);
    auto slider1 = std::dynamic_pointer_cast<ui::Slider>(result.elements["slider1"]);

    // Set additional callbacks (for elements that support runtime changes)
    if (toggle1) {
        toggle1->set_on_toggle([](bool checked) {
            std::cout << "Feature " << (checked ? "enabled" : "disabled") << " from JSON UI!" << std::endl;
        });
    }

    if (slider1) {
        slider1->set_on_value_changed(
            [](float value) { std::cout << "Volume changed to " << value << " from JSON UI!" << std::endl; });
    }

    std::cout << "\n=== JSON UI Demo ===" << std::endl;
    std::cout << "ESC - Exit" << std::endl;
    std::cout << "Interact with the UI loaded from JSON!" << std::endl;
    std::cout << "====================\n" << std::endl;

    // Mouse state
    double mouse_x = 0.0, mouse_y = 0.0;
    bool mouse_pressed = false;

    engine.run([&](float dt) {
        // Handle window resize
        int width, height;
        glfwGetFramebufferSize(engine.get_window(), &width, &height);
        result.canvas->resize(width, height);

        // Get mouse position
        glfwGetCursorPos(engine.get_window(), &mouse_x, &mouse_y);

        // Handle mouse button
        int mouse_state = glfwGetMouseButton(engine.get_window(), GLFW_MOUSE_BUTTON_LEFT);
        bool currently_pressed = (mouse_state == GLFW_PRESS);

        if (currently_pressed && !mouse_pressed) {
            // Mouse just pressed
            if (button1)
                button1->handle_mouse_button(mouse_x, mouse_y, true, width, height);
            if (button2)
                button2->handle_mouse_button(mouse_x, mouse_y, true, width, height);
            if (toggle1)
                toggle1->handle_click(mouse_x, mouse_y, width, height);
            if (slider1)
                slider1->handle_mouse_button(mouse_x, mouse_y, true, width, height);
        } else if (!currently_pressed && mouse_pressed) {
            // Mouse just released
            if (button1)
                button1->handle_mouse_button(mouse_x, mouse_y, false, width, height);
            if (button2)
                button2->handle_mouse_button(mouse_x, mouse_y, false, width, height);
            if (slider1)
                slider1->handle_mouse_button(mouse_x, mouse_y, false, width, height);
        }

        // Handle mouse drag for sliders
        if (currently_pressed && slider1) {
            slider1->handle_mouse_drag(mouse_x, mouse_y, width, height);
        }

        mouse_pressed = currently_pressed;

        // Handle mouse move for button hover
        if (button1)
            button1->handle_mouse_move(mouse_x, mouse_y, width, height);
        if (button2)
            button2->handle_mouse_move(mouse_x, mouse_y, width, height);

        // Handle keyboard
        if (engine.is_key_just_pressed(core::Key::Esc)) {
            engine.shutdown();
        }

        // Clear and render
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        result.canvas->render();
    });

    engine.free();
    return 0;
}
