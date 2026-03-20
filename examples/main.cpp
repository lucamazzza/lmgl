/*!
 * @file main.cpp
 * @brief LMGL Sandbox - PBR Materials & Lighting Demo
 */

#include "lmgl/assets/model_loader.hpp"
#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/renderer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/light.hpp"
#include "lmgl/scene/material.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"
#include "lmgl/scene/skybox.hpp"
#include "lmgl/ui/canvas.hpp"
#include "lmgl/ui/font.hpp"
#include "lmgl/ui/text.hpp"
#include "lmgl/ui/toggle.hpp"
#include "lmgl/ui/button.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <iostream>
#include <memory>

int main() {
  using namespace lmgl;

  auto &engine = core::Engine::get_instance();
  if (!engine.init(1920, 1080, "LMGL - Scene Viewer", true, false)) {
    std::cerr << "Failed to initialize engine!" << std::endl;
    return -1;
  }

  // Create UI Canvas
  auto canvas = std::make_shared<ui::Canvas>(1920, 1080);

  // Load font
  auto &font_mgr = ui::FontManager::get();
  auto ui_font = font_mgr.load(
      "ui_font", "/Users/lucamazza/Library/Fonts/IBMPlexMono-Text.ttf", 14);
  auto ui_font_small =
      font_mgr.load("ui_font_small",
                    "/Users/lucamazza/Library/Fonts/IBMPlexMono-Text.ttf", 11);

  if (!ui_font || !ui_font_small) {
    std::cerr << "Warning: Failed to load UI fonts, UI overlay disabled"
              << std::endl;
  } else {
    std::cout << "UI fonts loaded successfully!" << std::endl;
  }

  // Create FPS counter text
  auto fps_text = std::make_shared<ui::Text>("FPS: 0", "FPSText");
  if (ui_font_small) {
    fps_text->set_font(ui_font_small);
    fps_text->set_color(glm::vec4(0.1f, 1.0f, 0.1f, 1.0f));
    fps_text->set_position(glm::vec2(10.0f, 0.0f));
    fps_text->set_anchor(ui::Anchor::TopLeft);
    fps_text->set_render_order(1);
    canvas->add_element(fps_text);
  }

  // Individual control lines
  std::vector<std::shared_ptr<ui::Text>> control_lines;
  std::vector<std::string> control_texts = {"WASD  Move Camera",
                                             "Tab   Toggle Mouse Mode",
                                             "U     Toggle UI"};

  // UI Controls
  auto toggle_skybox =
      std::make_shared<ui::Toggle>("Skybox", "SkyboxToggle");
  toggle_skybox->get_text()->set_font(ui_font_small);
  toggle_skybox->set_position(glm::vec2(10.0f, -30.0f));
  toggle_skybox->set_anchor(ui::Anchor::BottomLeft);
  toggle_skybox->set_checked(true);
  toggle_skybox->set_box_size(10.0f);
  toggle_skybox->set_render_order(1);
  canvas->add_element(toggle_skybox);

  auto toggle_shadows =
      std::make_shared<ui::Toggle>("Shadows", "ShadowsToggle");
  toggle_shadows->get_text()->set_font(ui_font_small);
  toggle_shadows->set_position(glm::vec2(10.0f, -50.0f));
  toggle_shadows->set_anchor(ui::Anchor::BottomLeft);
  toggle_shadows->set_checked(true);
  toggle_shadows->set_box_size(10.0f);
  toggle_shadows->set_render_order(1);
  canvas->add_element(toggle_shadows);

  auto toggle_bloom =
      std::make_shared<ui::Toggle>("Bloom", "BloomToggle");
  toggle_bloom->get_text()->set_font(ui_font_small);
  toggle_bloom->set_position(glm::vec2(10.0f, -70.0f));
  toggle_bloom->set_anchor(ui::Anchor::BottomLeft);
  toggle_bloom->set_checked(false);
  toggle_bloom->set_box_size(10.0f);
  toggle_bloom->set_render_order(1);
  canvas->add_element(toggle_bloom);

  auto toggle_point_shadows =
      std::make_shared<ui::Toggle>("Point Shadows", "PointShadowsToggle");
  toggle_point_shadows->get_text()->set_font(ui_font_small);
  toggle_point_shadows->set_position(glm::vec2(10.0f, -90.0f));
  toggle_point_shadows->set_anchor(ui::Anchor::BottomLeft);
  toggle_point_shadows->set_checked(true);
  toggle_point_shadows->set_box_size(10.0f);
  toggle_point_shadows->set_render_order(1);
  canvas->add_element(toggle_point_shadows);

  auto toggle_dir_shadows =
      std::make_shared<ui::Toggle>("Directional Shadows", "DirShadowsToggle");
  toggle_dir_shadows->get_text()->set_font(ui_font_small);
  toggle_dir_shadows->set_position(glm::vec2(10.0f, -110.0f));
  toggle_dir_shadows->set_anchor(ui::Anchor::BottomLeft);
  toggle_dir_shadows->set_checked(true);
  toggle_dir_shadows->set_box_size(10.0f);
  toggle_dir_shadows->set_render_order(1);
  canvas->add_element(toggle_dir_shadows);

  auto btn_solid = std::make_shared<ui::Button>("Solid", "SolidButton");
  btn_solid->get_text()->set_font(ui_font_small);
  btn_solid->set_position(glm::vec2(10.0f, -150.0f));
  btn_solid->set_size(glm::vec2(80.0f, 25.0f));
  btn_solid->set_anchor(ui::Anchor::BottomLeft);
  btn_solid->set_render_order(1);
  canvas->add_element(btn_solid);

  auto btn_wireframe =
      std::make_shared<ui::Button>("Wireframe", "WireframeButton");
  btn_wireframe->get_text()->set_font(ui_font_small);
  btn_wireframe->set_position(glm::vec2(10.0f, -180.0f));
  btn_wireframe->set_size(glm::vec2(80.0f, 25.0f));
  btn_wireframe->set_anchor(ui::Anchor::BottomLeft);
  btn_wireframe->set_render_order(1);
  canvas->add_element(btn_wireframe);

  auto btn_points = std::make_shared<ui::Button>("Points", "PointsButton");
  btn_points->get_text()->set_font(ui_font_small);
  btn_points->set_position(glm::vec2(10.0f, -210.0f));
  btn_points->set_size(glm::vec2(80.0f, 25.0f));
  btn_points->set_anchor(ui::Anchor::BottomLeft);
  btn_points->set_render_order(1);
  canvas->add_element(btn_points);

  float y_offset = -270.0f;
  for (const auto &text : control_texts) {
    auto line = std::make_shared<ui::Text>(text, "ControlLine");
    if (ui_font_small) {
      line->set_font(ui_font_small);
      line->set_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
      line->set_position(glm::vec2(10.0f, y_offset));
      line->set_anchor(ui::Anchor::BottomLeft);
      line->set_render_order(1);
      canvas->add_element(line);
      control_lines.push_back(line);
    }
    y_offset += 18.0f;
  }

  // Load PBR shader
  auto pbr_shader = renderer::Shader::from_glsl_file("shaders/pbr.glsl");

  // Create scene
  auto scene = std::make_shared<scene::Scene>("PBR Demo Scene");

  // Try to load a skybox (will gracefully fail if files don't exist)
  auto cubemap =
      scene::Cubemap::from_equirectangular("examples/assets/skybox.hdr");
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
      60.0f, engine.get_aspect_ratio(), 0.1f, 100.0f);
  camera->set_position(glm::vec3(0.0f, 2.0f, 8.0f));
  camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

  // Create renderer
  auto renderer = std::make_unique<renderer::Renderer>();
  renderer->set_tone_map_mode(1); // reinhard



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
  ground_node->set_position(glm::vec3(0.0f, -5.0f, 0.0f));
  scene->get_root()->add_child(ground_node);

  auto options = assets::ModelLoadOptions();
  options.optimize_meshes = true;
  options.flip_uvs = true;
  auto rifle = assets::ModelLoader::load("examples/assets/ratchet_wrench_4k.gltf",
                                         pbr_shader, options);
  if (!rifle) {
    std::cerr << "Failed to load wrench model!" << std::endl;
  } else {
    rifle->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    rifle->set_scale(10.0f);
    scene->get_root()->add_child(rifle);
    std::cout << "Wrench model loaded and added to scene" << std::endl;
  }

  // CREATE LIGHTS

  // Directional light (sun)
  auto sun = scene::Light::create_directional(glm::vec3(0.5f, -1.0f, -0.3f),
                                              glm::vec3(1.0f, 0.95f, 0.9f));
  sun->set_intensity(3.0f);
  scene->add_light(sun);

  // Fill light (soft ambient from opposite direction)
  auto fill_light = scene::Light::create_directional(glm::vec3(-0.3f, 0.5f, 0.5f),
                                                     glm::vec3(0.6f, 0.7f, 1.0f));
  fill_light->set_intensity(0.5f);
  scene->add_light(fill_light);

  // Point light (orange accent)
  auto point_light = scene::Light::create_point(
      glm::vec3(3.0f, 3.0f, 3.0f), 15.0f, glm::vec3(1.0f, 0.7f, 0.4f));
  point_light->set_intensity(15.0f);
  scene->add_light(point_light);

  // Animation state
  float time = 0.0f;
  renderer::RenderMode current_mode = renderer::RenderMode::Solid;
  auto skybox_ref = scene->get_skybox(); // Store reference for toggling
  bool render_skybox = (skybox_ref != nullptr);
  bool show_ui = true;
  bool enable_point_shadows = true;
  bool enable_directional_shadows = true;

  // Configure scene shadow settings
  scene->set_shadows_enabled(true);
  scene->set_shadow_resolution(4096);

  // Camera movement - free camera setup
  glm::vec3 camera_pos = camera->get_position();
  float camera_yaw = -90.0f;
  float camera_pitch = 0.0f;

  // Mouse mode state - start with free camera
  bool mouse_locked = true;
  engine.set_cursor_mode(core::CursorMode::Disabled);

  // Handle window resize
  engine.set_resize_callback([&](int width, int height) {
    camera->set_aspect(engine.get_aspect_ratio());
    renderer->resize(width, height);
    canvas->resize(width, height);
    std::cout << "Window resized: " << width << "x" << height
              << " (aspect: " << engine.get_aspect_ratio() << ")" << std::endl;
  });

  renderer->set_bloom_enabled(false);
  // Main loop
  engine.run([&](float dt) {
    time += dt;

    // Handle UI clicks with single-click tracking (only when mouse is unlocked)
    if (!mouse_locked && engine.is_mouse_button_just_pressed(core::MouseButton::Left)) {
      float mx = engine.get_mouse_x();
      float my = engine.get_mouse_y();
      float cw = canvas->get_width();
      float ch = canvas->get_height();
      toggle_skybox->handle_click(mx, my, cw, ch);
      toggle_shadows->handle_click(mx, my, cw, ch);
      toggle_bloom->handle_click(mx, my, cw, ch);
      toggle_point_shadows->handle_click(mx, my, cw, ch);
      toggle_dir_shadows->handle_click(mx, my, cw, ch);
      btn_solid->handle_mouse_button(mx, my, true, cw, ch);
      btn_wireframe->handle_mouse_button(mx, my, true, cw, ch);
      btn_points->handle_mouse_button(mx, my, true, cw, ch);
    }

    if (!mouse_locked && engine.is_mouse_button_just_released(core::MouseButton::Left)) {
      float mx = engine.get_mouse_x();
      float my = engine.get_mouse_y();
      float cw = canvas->get_width();
      float ch = canvas->get_height();
      
      btn_solid->handle_mouse_button(mx, my, false, cw, ch);
      btn_wireframe->handle_mouse_button(mx, my, false, cw, ch);
      btn_points->handle_mouse_button(mx, my, false, cw, ch);
    }

    // Handle mouse movement for button hover states (only when unlocked)
    if (!mouse_locked) {
      float mx = engine.get_mouse_x();
      float my = engine.get_mouse_y();
      float cw = canvas->get_width();
      float ch = canvas->get_height();
      btn_solid->handle_mouse_move(mx, my, cw, ch);
      btn_wireframe->handle_mouse_move(mx, my, cw, ch);
      btn_points->handle_mouse_move(mx, my, cw, ch);
    }

    // INPUT HANDLING

    if (engine.is_key_just_pressed(core::Key::Esc)) {
      engine.shutdown();
    }

    if (engine.is_key_just_pressed(core::Key::F)) {
      engine.set_fullscreen(!engine.is_fullscreen());
    }

    // Toggle mouse mode with Tab key
    if (engine.is_key_just_pressed(core::Key::Tab)) {
      mouse_locked = !mouse_locked;
      engine.set_cursor_mode(mouse_locked ? core::CursorMode::Disabled : core::CursorMode::Normal);
      std::cout << "Mouse " << (mouse_locked ? "locked (free camera)" : "unlocked (UI mode)") << std::endl;
    }

    if (engine.is_key_just_pressed(core::Key::U)) {
      show_ui = !show_ui;
      canvas->set_visible(show_ui);
      std::cout << "UI overlay " << (show_ui ? "enabled" : "disabled")
                << std::endl;
    }

    // UI Callbacks
    toggle_skybox->set_on_toggle([&](bool checked) {
      if (skybox_ref) {
        render_skybox = checked;
        scene->set_skybox(render_skybox ? skybox_ref : nullptr);
        std::cout << "Skybox " << (render_skybox ? "enabled" : "disabled")
                  << std::endl;
      }
    });

    toggle_shadows->set_on_toggle([&](bool checked) {
      scene->set_shadows_enabled(checked);
      std::cout << "Shadows " << (checked ? "enabled" : "disabled")
                << std::endl;
    });

    toggle_bloom->set_on_toggle([&](bool checked) {
      renderer->set_bloom_enabled(checked);
      std::cout << "Bloom " << (checked ? "enabled" : "disabled") << std::endl;
    });

    toggle_point_shadows->set_on_toggle([&](bool checked) {
      enable_point_shadows = checked;
      std::cout << "Point shadows " << (checked ? "enabled" : "disabled") << std::endl;
    });

    toggle_dir_shadows->set_on_toggle([&](bool checked) {
      enable_directional_shadows = checked;
      std::cout << "Directional shadows " << (checked ? "enabled" : "disabled") << std::endl;
    });

    btn_solid->set_on_click([&]() {
      current_mode = renderer::RenderMode::Solid;
      renderer->set_render_mode(current_mode);
      std::cout << "Render mode: Solid" << std::endl;
    });

    btn_wireframe->set_on_click([&]() {
      current_mode = renderer::RenderMode::Wireframe;
      renderer->set_render_mode(current_mode);
      std::cout << "Render mode: Wireframe" << std::endl;
    });

    btn_points->set_on_click([&]() {
      current_mode = renderer::RenderMode::Points;
      renderer->set_render_mode(current_mode);
      std::cout << "Render mode: Points" << std::endl;
    });
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

    // Mouse look (only when locked)
    if (mouse_locked) {
      double dx, dy;
      engine.get_mouse_delta(dx, dy);

      camera_yaw += dx * 0.1f;
      camera_pitch -= dy * 0.1f;
      camera_pitch = glm::clamp(camera_pitch, -89.0f, 89.0f);
    }

    glm::vec3 direction;
    direction.x =
        cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
    direction.y = sin(glm::radians(camera_pitch));
    direction.z =
        sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));

    camera->set_position(camera_pos);
    camera->set_target(camera_pos + glm::normalize(direction));

    // Render
    engine.clear(0.05f, 0.05f, 0.1f);

    // Setup shadows automatically
    renderer->setup_shadows(scene, pbr_shader, enable_point_shadows, enable_directional_shadows);

    // Render scene with frustum culling (automatic)
    scene->update();
    renderer->render(scene, camera);

    // Update and render UI
    if (ui_font && fps_text) {
      fps_text->set_text("FPS: " +
                         std::to_string(static_cast<int>(engine.get_fps())));
    }
    canvas->render();

    // Update window title with stats
    static float title_timer = 0.0f;
    title_timer += dt;
    if (title_timer >= 0.5f) {
      std::string title =
          "LMGL Sandbox | Draw Calls: " +
          std::to_string(renderer->get_draw_calls()) +
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
