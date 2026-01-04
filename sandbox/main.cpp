/*!
 * @file main.cpp
 * @brief LMGL Engine Sandbox - Showcasing renderer, camera, and scene features
 */

#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/renderer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"

#include <cmath>
#include <iostream>
#include <memory>

int main() {
    using namespace lmgl;

    // Initialize engine
    auto &engine = core::Engine::get_instance();
    if (!engine.init(1280, 720, "LMGL Sandbox - Feature Demo")) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    std::cout << "\n=== LMGL Engine Sandbox ===" << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "  ✓ Scene graph with hierarchical transforms" << std::endl;
    std::cout << "  ✓ Camera system (perspective projection)" << std::endl;
    std::cout << "  ✓ Renderer with optimized render queue" << std::endl;
    std::cout << "  ✓ Procedural mesh generation" << std::endl;
    std::cout << "  ✓ Shader system with uniforms" << std::endl;
    std::cout << "  ✓ Real-time transformations" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << "  1   - Solid rendering" << std::endl;
    std::cout << "  2   - Wireframe mode" << std::endl;
    std::cout << "  3   - Point cloud mode" << std::endl;
    std::cout << "=============================\n" << std::endl;

    // Load shader
    auto shader = renderer::Shader::from_glsl_file("shaders/basic.glsl");

    // Create scene
    auto scene = std::make_shared<scene::Scene>("Demo Scene");

    // Create camera
    auto camera = std::make_shared<scene::Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    camera->set_position(glm::vec3(5.0f, 5.0f, 8.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

    // Create renderer
    auto renderer = std::make_unique<renderer::Renderer>();

    // === Create Scene Objects ===

    // Center rotating cube
    auto center_cube = scene::Mesh::create_cube(shader);
    auto center_node = std::make_shared<scene::Node>("Center Cube");
    center_node->set_mesh(center_cube);
    center_node->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
    scene->get_root()->add_child(center_node);

    // Orbiting satellite cubes (children of center cube)
    for (int i = 0; i < 3; ++i) {
        auto satellite = scene::Mesh::create_cube(shader);
        auto sat_node = std::make_shared<scene::Node>("Satellite " + std::to_string(i));
        sat_node->set_mesh(satellite);
        sat_node->set_scale(0.4f);
        
        float angle = (i / 3.0f) * 2.0f * M_PI;
        sat_node->set_position(glm::vec3(std::cos(angle) * 2.0f, 0.0f, std::sin(angle) * 2.0f));
        
        center_node->add_child(sat_node);
    }

    // Ground plane
    auto ground = scene::Mesh::create_quad(shader, 12.0f, 12.0f);
    auto ground_node = std::make_shared<scene::Node>("Ground");
    ground_node->set_mesh(ground);
    ground_node->set_rotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    scene->get_root()->add_child(ground_node);

    // Sphere on the side
    auto sphere = scene::Mesh::create_sphere(shader, 0.6f, 32, 32);
    auto sphere_node = std::make_shared<scene::Node>("Sphere");
    sphere_node->set_mesh(sphere);
    sphere_node->set_position(glm::vec3(-3.0f, 0.6f, 0.0f));
    scene->get_root()->add_child(sphere_node);

    // Tall cube pillar
    auto pillar = scene::Mesh::create_cube(shader);
    auto pillar_node = std::make_shared<scene::Node>("Pillar");
    pillar_node->set_mesh(pillar);
    pillar_node->set_position(glm::vec3(3.0f, 1.5f, 0.0f));
    pillar_node->set_scale(glm::vec3(0.5f, 3.0f, 0.5f));
    scene->get_root()->add_child(pillar_node);

    // Animation variables
    float time = 0.0f;
    renderer::RenderMode current_mode = renderer::RenderMode::Solid;

    // Stats tracking
    float fps_timer = 0.0f;
    int frame_count = 0;

    std::cout << "Scene created with " << (scene->get_root()->get_children().size()) 
              << " root objects (7 total meshes)" << std::endl;
    std::cout << "Rendering started...\n" << std::endl;

    // Main render loop
    engine.run([&](float delta_time) {
        time += delta_time;

        // Handle input
        auto window = engine.get_window();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            current_mode = renderer::RenderMode::Solid;
            renderer->set_render_mode(current_mode);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            current_mode = renderer::RenderMode::Wireframe;
            renderer->set_render_mode(current_mode);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            current_mode = renderer::RenderMode::Points;
            renderer->set_render_mode(current_mode);
        }

        // Animate center cube rotation (parent transforms affect children!)
        center_node->set_rotation(glm::vec3(
            time * 20.0f,
            time * 30.0f,
            time * 10.0f
        ));

        // Animate sphere - bounce up and down
        sphere_node->set_position(glm::vec3(
            -3.0f, 
            0.6f + std::sin(time * 3.0f) * 0.3f,
            0.0f
        ));

        // Animate camera - orbit around scene
        float cam_angle = time * 0.2f;
        float cam_radius = 8.0f;
        camera->set_position(glm::vec3(
            std::cos(cam_angle) * cam_radius,
            5.0f,
            std::sin(cam_angle) * cam_radius
        ));
        camera->set_target(glm::vec3(0.0f, 0.5f, 0.0f));

        // Clear and render
        engine.clear(0.1f, 0.1f, 0.15f);
        renderer->render(scene, camera);

        // FPS counter (every second)
        fps_timer += delta_time;
        frame_count++;

        if (fps_timer >= 1.0f) {
            std::cout << "FPS: " << frame_count 
                      << " | Draw Calls: " << renderer->get_draw_calls()
                      << " | Triangles: " << renderer->get_triangles_count()
                      << " | Mode: ";

            switch (current_mode) {
                case renderer::RenderMode::Solid:
                    std::cout << "Solid";
                    break;
                case renderer::RenderMode::Wireframe:
                    std::cout << "Wireframe";
                    break;
                case renderer::RenderMode::Points:
                    std::cout << "Points";
                    break;
            }

            std::cout << std::endl;

            fps_timer = 0.0f;
            frame_count = 0;
        }
    });

    // Cleanup
    engine.free();
    std::cout << "\nEngine shut down successfully." << std::endl;
    return 0;
}
