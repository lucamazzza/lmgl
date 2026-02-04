/*!
 * @file main.cpp
 * @brief LMGL Sandbox - PBR Materials & Lighting Demo
 */

#include "lmgl/assets/model_loader.hpp"
#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/renderer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/shadow_map.hpp"
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"
#include "lmgl/scene/material.hpp"
#include "lmgl/scene/light.hpp"
#include "lmgl/scene/skybox.hpp"

#include <iostream>
#include <memory>

int main() {
    using namespace lmgl;

    // Initialize engine with aspect-ratio aware settings
    auto &engine = core::Engine::get_instance();
    if (!engine.init(1280, 720, "LMGL - PBR & Lighting Demo", true, false)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    std::cout << "\n=== LMGL PBR Demo ===" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  ESC       - Exit" << std::endl;
    std::cout << "  1/2/3     - Render modes" << std::endl;
    std::cout << "  7/8/9     - Tone Map modes" << std::endl;
    std::cout << "  F         - Toggle fullscreen" << std::endl;
    std::cout << "  B         - Toggle skybox" << std::endl;
    std::cout << "  H         - Toggle shadows" << std::endl;
    std::cout << "  WASD      - Move camera" << std::endl;
    std::cout << "  Mouse     - Look around" << std::endl;
    std::cout << "========================\n" << std::endl;

    // Load PBR shader
    auto pbr_shader = renderer::Shader::from_glsl_file("shaders/pbr.glsl");

    // Create scene
    auto scene = std::make_shared<scene::Scene>("PBR Demo Scene");

    // Try to load a skybox (will gracefully fail if files don't exist)
    auto cubemap = scene::Cubemap::from_equirectangular("sandbox/assets/skybox.hdr");
    if (cubemap) {
        auto skybox = std::make_shared<scene::Skybox>(cubemap);
        skybox->set_exposure(1.0f);
        scene->set_skybox(skybox);
        std::cout << "Skybox loaded successfully!" << std::endl;
    } else {
        std::cout << "Skybox not loaded (missing assets/skybox.hdr)" << std::endl;
    }

    // Create camera with aspect ratio from engine
    auto camera = std::make_shared<scene::Camera>(
        60.0f, 
        engine.get_aspect_ratio(), 
        0.1f, 
        100.0f
    );
    camera->set_position(glm::vec3(0.0f, 2.0f, 8.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

    // Create renderer
    auto renderer = std::make_unique<renderer::Renderer>();
    renderer->set_tone_map_mode(1); // reinhard

    // Create shadow map for directional light
    auto shadow_renderer = std::make_unique<renderer::ShadowRenderer>();
    auto shadow_map = std::make_shared<renderer::ShadowMap>(4096, 4096); // Higher resolution
    std::cout << "Shadow map created (4096x4096)" << std::endl;

    // === Create Materials ===

    // Shiny metal material
    auto metal_material = std::make_shared<scene::Material>("Metal");
    metal_material->set_albedo(glm::vec3(0.8f, 0.8f, 0.8f));
    metal_material->set_metallic(1.0f);
    metal_material->set_roughness(0.2f);

    // Rough plastic material
    auto plastic_material = std::make_shared<scene::Material>("Plastic");
    plastic_material->set_albedo(glm::vec3(0.2f, 0.5f, 0.8f));
    plastic_material->set_metallic(0.0f);
    plastic_material->set_roughness(0.7f);

    // Gold material
    auto gold_material = std::make_shared<scene::Material>("Gold");
    gold_material->set_albedo(glm::vec3(1.0f, 0.782f, 0.344f));
    gold_material->set_metallic(1.0f);
    gold_material->set_roughness(0.3f);

    // Emissive material
    auto emissive_material = std::make_shared<scene::Material>("Emissive");
    emissive_material->set_albedo(glm::vec3(0.1f, 0.1f, 0.1f));
    emissive_material->set_emissive(glm::vec3(2.0f, 0.5f, 0.1f));

    // === Create Scene Objects ===

    // Metal sphere (left)
    auto metal_sphere = scene::Mesh::create_sphere(pbr_shader, 0.8f, 32, 32);
    metal_sphere->set_material(metal_material);
    auto metal_node = std::make_shared<scene::Node>("Metal Sphere");
    metal_node->set_mesh(metal_sphere);
    metal_node->set_position(glm::vec3(-3.0f, 1.0f, 0.0f));
    scene->get_root()->add_child(metal_node);

    // Plastic sphere (center)
    auto plastic_sphere = scene::Mesh::create_sphere(pbr_shader, 0.8f, 32, 32);
    plastic_sphere->set_material(plastic_material);
    auto plastic_node = std::make_shared<scene::Node>("Plastic Sphere");
    plastic_node->set_mesh(plastic_sphere);
    plastic_node->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
    scene->get_root()->add_child(plastic_node);

    // Gold sphere (right)
    auto gold_sphere = scene::Mesh::create_sphere(pbr_shader, 0.8f, 32, 32);
    gold_sphere->set_material(gold_material);
    auto gold_node = std::make_shared<scene::Node>("Gold Sphere");
    gold_node->set_mesh(gold_sphere);
    gold_node->set_position(glm::vec3(3.0f, 1.0f, 0.0f));
    scene->get_root()->add_child(gold_node);

    // Emissive cube (light source visualization)
    auto emissive_cube = scene::Mesh::create_cube(pbr_shader);
    emissive_cube->set_material(emissive_material);
    auto emissive_node = std::make_shared<scene::Node>("Emissive Cube");
    emissive_node->set_mesh(emissive_cube);
    emissive_node->set_position(glm::vec3(0.0f, 3.0f, 0.0f));
    emissive_node->set_scale(0.3f);
    scene->get_root()->add_child(emissive_node);

    // Ground plane
    auto ground_material = std::make_shared<scene::Material>("Ground");
    ground_material->set_albedo(glm::vec3(0.3f, 0.3f, 0.3f));
    ground_material->set_metallic(0.0f);
    ground_material->set_roughness(0.9f);

    auto ground = scene::Mesh::create_quad(pbr_shader, 20.0f, 20.0f);
    ground->set_material(ground_material);
    auto ground_node = std::make_shared<scene::Node>("Ground");
    ground_node->set_mesh(ground);
    ground_node->set_rotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    scene->get_root()->add_child(ground_node);

    auto options = assets::ModelLoadOptions();
    options.optimize_meshes = true;
    options.flip_uvs = true;
    auto rifle = assets::ModelLoader::load("sandbox/assets/rifle.obj", pbr_shader, options);
    rifle->set_position(glm::vec3(2.0f, 2.0f, -2.0f));
    rifle->set_scale(0.05f);
    scene->get_root()->add_child(rifle);

    // === Create Lights ===

    // Directional light (sun)
    auto sun = scene::Light::create_directional(
        glm::vec3(0.3f, -1.0f, -0.5f),
        glm::vec3(1.0f, 0.95f, 0.9f)
    );
    sun->set_intensity(2.0f);
    scene->add_light(sun);

    // Point light (orange)
    auto point_light = scene::Light::create_point(
        glm::vec3(0.0f, 3.0f, 0.0f),
        10.0f,
        glm::vec3(1.0f, 0.5f, 0.2f)
    );
    point_light->set_intensity(20.0f);
    scene->add_light(point_light);

    // Animation state
    float time = 0.0f;
    renderer::RenderMode current_mode = renderer::RenderMode::Solid;
    bool camera_free_look = false;
    auto skybox_ref = scene->get_skybox(); // Store reference for toggling
    bool render_skybox = (skybox_ref != nullptr);
    bool render_shadows = true;

    // Camera movement
    glm::vec3 camera_pos = camera->get_position();
    float camera_yaw = -90.0f;
    float camera_pitch = 0.0f;

    // Handle window resize
    engine.set_resize_callback([&](int width, int height) {
        camera->set_aspect(engine.get_aspect_ratio());
        renderer->resize(width, height);
        std::cout << "Window resized: " << width << "x" << height 
                  << " (aspect: " << engine.get_aspect_ratio() << ")" << std::endl;
    });

    // Main loop
    engine.run([&](float dt) {
        time += dt;

        // === Input Handling (no GLFW!) ===

        if (engine.is_key_just_pressed(core::Key::Esc)) {
            engine.shutdown();
        }

        if (engine.is_key_just_pressed(core::Key::F)) {
            engine.set_fullscreen(!engine.is_fullscreen());
        }

        if (engine.is_key_just_pressed(core::Key::Key1)) {
            current_mode = renderer::RenderMode::Solid;
            renderer->set_render_mode(current_mode);
        }
        if (engine.is_key_just_pressed(core::Key::Key2)) {
            current_mode = renderer::RenderMode::Wireframe;
            renderer->set_render_mode(current_mode);
        }
        if (engine.is_key_just_pressed(core::Key::Key3)) {
            current_mode = renderer::RenderMode::Points;
            renderer->set_render_mode(current_mode);
        }
        if (engine.is_key_just_pressed(core::Key::Key7)) {
            renderer->set_tone_map_mode(0);
        }
        if (engine.is_key_just_pressed(core::Key::Key8)) {
            renderer->set_tone_map_mode(1);
        }
        if (engine.is_key_just_pressed(core::Key::Key9)) {
            renderer->set_tone_map_mode(2);
        }
        if (engine.is_key_just_pressed(core::Key::B)) {
            if (skybox_ref) {
                render_skybox = !render_skybox;
                scene->set_skybox(render_skybox ? skybox_ref : nullptr);
                std::cout << "Skybox " << (render_skybox ? "enabled" : "disabled") << std::endl;
            }
        }
        if (engine.is_key_just_pressed(core::Key::H)) {
            render_shadows = !render_shadows;
            std::cout << "Shadows " << (render_shadows ? "enabled" : "disabled") << std::endl;
        }
        if (engine.is_key_just_pressed(core::Key::N)) {
            static bool bloom = true;
            bloom = !bloom;
            renderer->set_bloom_enabled(bloom);
            std::cout << "Bloom " << (bloom ? "enabled" : "disabled") << std::endl;
        }
        // Camera movement (WASD)
        float cam_speed = 5.0f * dt;
        glm::vec3 forward = glm::normalize(camera->get_target() - camera_pos);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

        if (engine.is_key_pressed(core::Key::W)) {
            camera_pos += forward * cam_speed;
        }
        if (engine.is_key_pressed(core::Key::S)) {
            camera_pos -= forward * cam_speed;
        }
        if (engine.is_key_pressed(core::Key::A)) {
            camera_pos -= right * cam_speed;
        }
        if (engine.is_key_pressed(core::Key::D)) {
            camera_pos += right * cam_speed;
        }

        // Mouse look (if enabled)
        if (engine.is_mouse_button_pressed(core::MouseButton::Right)) {
            if (!camera_free_look) {
                engine.set_cursor_mode(core::CursorMode::Disabled);
                camera_free_look = true;
            }

            double dx, dy;
            engine.get_mouse_delta(dx, dy);

            camera_yaw += dx * 0.1f;
            camera_pitch -= dy * 0.1f;
            camera_pitch = glm::clamp(camera_pitch, -89.0f, 89.0f);

            glm::vec3 direction;
            direction.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
            direction.y = sin(glm::radians(camera_pitch));
            direction.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));

            camera->set_position(camera_pos);
            camera->set_target(camera_pos + glm::normalize(direction));
        } else {
            if (camera_free_look) {
                engine.set_cursor_mode(core::CursorMode::Normal);
                camera_free_look = false;
            }

            // Orbital camera when not in free look
            float cam_angle = time * 0.3f;
            float cam_radius = 8.0f;
            camera_pos = glm::vec3(
                std::cos(cam_angle) * cam_radius,
                3.0f,
                std::sin(cam_angle) * cam_radius
            );
            camera->set_position(camera_pos);
            camera->set_target(glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Animate spheres
        metal_node->set_rotation(glm::vec3(0.0f, time * 30.0f, 0.0f));
        plastic_node->set_rotation(glm::vec3(0.0f, time * 20.0f, 0.0f));
        gold_node->set_rotation(glm::vec3(0.0f, time * 40.0f, 0.0f));

        // Animate point light
        float light_angle = time * 2.0f;
        point_light->set_position(glm::vec3(
            std::cos(light_angle) * 3.0f,
            3.0f,
            std::sin(light_angle) * 3.0f
        ));
        emissive_node->set_position(point_light->get_position());

        // Render
        engine.clear(0.05f, 0.05f, 0.1f);
        
        // Shadow pass (if enabled)
        glm::mat4 light_space_matrix(1.0f);
        if (render_shadows) {
            shadow_renderer->render_directional_shadow(scene, sun, shadow_map);
            // Get light space matrix for shader - centered on scene with larger radius
            light_space_matrix = shadow_renderer->get_light_space_matrix(sun, glm::vec3(0.0f, 2.0f, 0.0f), 20.0f);
            
            // Bind shadow map and set uniforms on PBR shader
            pbr_shader->bind();
            shadow_map->bind_texture(15); // Use texture slot 15 for shadows
            pbr_shader->set_int("u_ShadowMap", 15);
            pbr_shader->set_int("u_UseShadows", 1);
            pbr_shader->set_mat4("u_LightSpaceMatrix", light_space_matrix);
        } else {
            // Disable shadows
            pbr_shader->bind();
            pbr_shader->set_int("u_UseShadows", 0);
        }
        
        // Render scene with frustum culling (automatic)
        scene->update();
        renderer->render(scene, camera);

        // Update window title with stats
        static float title_timer = 0.0f;
        title_timer += dt;
        if (title_timer >= 0.5f) {
            std::string title = "LMGL PBR Demo | FPS: " + std::to_string(static_cast<int>(engine.get_fps())) +
                              " | Draw Calls: " + std::to_string(renderer->get_draw_calls()) +
                              " | Tris: " + std::to_string(renderer->get_triangles_count());
            engine.set_title(title);
            title_timer = 0.0f;
        }
    });

    // Cleanup
    engine.free();
    std::cout << "\nEngine shut down successfully." << std::endl;
    return 0;
}
