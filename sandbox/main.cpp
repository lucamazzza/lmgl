/*!
 * @file main.cpp
 * @brief Simple sandbox application demonstrating the LMGL engine
 */

#include "lmgl/core/engine.hpp"
#include <iostream>
#include <cmath>

int main() {
    using namespace lmgl::core;
    
    // Get the singleton engine instance
    auto& engine = Engine::get_instance();
    
    // Initialize the engine with a 1280x720 window
    if (!engine.init(1280, 720, "LMGL Sandbox")) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    // Color animation variables
    float time = 0.0f;
    
    // Run the main loop
    engine.run([&](float delta_time) {
        time += delta_time;
        
        // Animate background color using sine waves
        float r = 0.2f + 0.3f * std::sin(time * 0.5f);
        float g = 0.3f + 0.3f * std::sin(time * 0.7f);
        float b = 0.3f + 0.3f * std::sin(time * 0.9f);
        
        // Clear the screen with animated color
        engine.clear(r, g, b); 
        
        // Print FPS every second
        static float fps_timer = 0.0f;
        static int frame_count = 0;
        fps_timer += delta_time;
        frame_count++;
        
        if (fps_timer >= 1.0f) {
            std::cout << "FPS: " << frame_count << std::endl;
            fps_timer = 0.0f;
            frame_count = 0;
        }
    });
    
    // Cleanup
    engine.free();
    std::cout << "Engine shut down successfully." << std::endl;
    return 0;
}
