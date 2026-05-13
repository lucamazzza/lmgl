/*!
 * @file ovr_stereo_demo.cpp
 * @brief LMGL Sandbox - PBR Materials & Lighting Demo in stereo mode
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
#include "lmgl/vr/ovr_backend.hpp"
#include "lmgl/vr/leap.h"
#include "lmgl/vr/handInput.h"

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cctype>
#include <functional>
#include <limits>
#include <unordered_map>

int main() {
  using namespace lmgl;
  constexpr float kWorldScale = 10.0f;

  auto &engine = core::Engine::get_instance();
  if (!engine.init(1920, 1080, "LMGL - OVR Stereo Viewer", true, false)) {
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
    skybox->set_exposure(0.45f);
    scene->set_skybox(skybox);
    std::cout << "Skybox loaded successfully!" << std::endl;
  } else {
    std::cout << "Skybox not loaded (missing assets/skybox.hdr)" << std::endl;
  }

  // Create camera with aspect ratio from engine
  auto camera = std::make_shared<scene::Camera>(
      60.0f, engine.get_aspect_ratio() * 0.5f, 0.1f, 1000.0f);
  camera->set_position(glm::vec3(0.0f, 2.0f, 20.0f) * kWorldScale);
  camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

  // Create renderer
  auto renderer = std::make_unique<renderer::Renderer>();
  renderer->set_tone_map_mode(2); // ACES
  renderer->set_exposure(0.65f);

  vr::OvrBackend ovr_backend;
  if (ovr_backend.initialize()) {
    std::cout << "OVR backend initialized";
    if (ovr_backend.has_runtime()) {
      std::cout << " (runtime connected)";
    } else {
      std::cout << " (fallback mode)";
    }
    std::cout << std::endl;
  } else {
    std::cout << "OVR backend init failed, using fallback stereo only" << std::endl;
  }

  auto leap = std::make_unique<vr::Leap>();
  if (!leap->init()) {
    std::cout << "Failed to initialize Leap Motion" << std::endl;
  }

  // Hands setup
  auto hand_material = std::make_shared<scene::Material>("Hand");
  hand_material->set_albedo(glm::vec3(0.9f, 0.75f, 0.6f));
  hand_material->set_roughness(0.8f);
  hand_material->set_metallic(0.0f);

  auto joint_mesh = scene::Mesh::create_sphere(pbr_shader, 0.3f * kWorldScale, 8, 8);
  joint_mesh->set_material(hand_material);

  const int JOINTS_PER_HAND = 26;
  std::vector<std::shared_ptr<scene::Node>> hand_nodes;

  for (int i = 0; i < 2 * JOINTS_PER_HAND; i++) {
    auto node = std::make_shared<scene::Node>("JointNode");
    node->set_mesh(joint_mesh);
    node->set_scale(1.0f);
    scene->get_root()->add_child(node);
    hand_nodes.push_back(node);
}

  auto options = assets::ModelLoadOptions();
  options.optimize_meshes = false;
  options.flip_uvs = true;
  auto rifle = assets::ModelLoader::load("examples/assets/Hanoi.gltf",
                                         pbr_shader, options);
  if (!rifle) {
    std::cerr << "Failed to load wrench model!" << std::endl;
  } else {
    rifle->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    rifle->set_scale(1.0f);
    scene->get_root()->add_child(rifle);
    rifle->update_transform(scene->get_root()->get_world_transform());
    std::cout << "Wrench model loaded and added to scene" << std::endl;
  }
  

  
  std::vector<std::shared_ptr<scene::Node>> disks;

  // navigate to tower1
  auto tower1 = rifle->get_children()[0]; // first child of hanoi
  std::function<void(std::shared_ptr<scene::Node>)> find_disks;
  find_disks = [&](std::shared_ptr<scene::Node> node) {
      if (node->get_name().find("disk") != std::string::npos) {
          disks.push_back(node);
          std::cout << "Found disk: " << node->get_name() << std::endl;
      }
      for (auto& child : node->get_children())
          find_disks(child);
  };
  find_disks(tower1);
  const auto parse_disk_order = [](const std::string& name) -> int {
    int value = 0;
    bool found_digit = false;
    for (unsigned char c : name) {
      if (std::isdigit(c)) {
        found_digit = true;
        value = value * 10 + (c - '0');
      }
    }
    return found_digit ? value : std::numeric_limits<int>::max();
  };
  std::unordered_map<scene::Node*, int> disk_order;
  std::unordered_map<std::string, glm::vec3> disk_original_scales;
  for (auto& disk : disks) {
      disk_order[disk.get()] = parse_disk_order(disk->get_name());
      disk_original_scales[disk->get_name()] = disk->get_scale();
      std::cout << "Saved scale for " << disk->get_name()
                << ": " << disk->get_scale().x << std::endl;
  }

  std::vector<std::shared_ptr<scene::Node>> towers;
  std::function<void(std::shared_ptr<scene::Node>)> find_towers;
  find_towers = [&](std::shared_ptr<scene::Node> node) {
    if (node->get_name().find("tower") != std::string::npos) {
      towers.push_back(node);
    }
    for (const auto& child : node->get_children()) {
      find_towers(child);
    }
  };
  find_towers(rifle);
  if (towers.empty()) {
    for (const auto& child : rifle->get_children()) {
      towers.push_back(child);
      if (towers.size() == 3) {
        break;
      }
    }
  }

  const auto get_world_pos = [](const std::shared_ptr<scene::Node>& node) -> glm::vec3 {
    return glm::vec3(node->get_world_transform()[3]);
  };

  const auto is_descendant_of =
      [](const std::shared_ptr<scene::Node>& node, const std::shared_ptr<scene::Node>& ancestor) -> bool {
    auto current = node;
    while (current) {
      if (current == ancestor) {
        return true;
      }
      current = current->get_parent();
    }
    return false;
  };

  float tower_base_y = std::numeric_limits<float>::max();
  for (const auto& disk : disks) {
    tower_base_y = std::min(tower_base_y, get_world_pos(disk).y);
  }
  if (tower_base_y == std::numeric_limits<float>::max()) {
    tower_base_y = 0.0f;
  }
  float disk_stack_step = 0.0f;
  for (size_t i = 0; i < disks.size(); i++) {
    for (size_t j = i + 1; j < disks.size(); j++) {
      const float diff = std::abs(get_world_pos(disks[i]).y - get_world_pos(disks[j]).y);
      if (diff > 0.001f && (disk_stack_step <= 0.0f || diff < disk_stack_step)) {
        disk_stack_step = diff;
      }
    }
  }
  if (disk_stack_step <= 0.0f) {
    disk_stack_step = 0.6f * kWorldScale;
  }

  // CREATE LIGHTS

  // Directional light (sun)
  auto sun = scene::Light::create_directional(glm::vec3(0.5f, -1.0f, -0.3f),
                                              glm::vec3(1.0f, 0.95f, 0.9f));
  sun->set_intensity(0.0f);
  scene->add_light(sun);

  // Fill light (soft ambient from opposite direction)
  auto fill_light = scene::Light::create_directional(glm::vec3(-0.3f, 0.5f, 0.5f),
                                                     glm::vec3(0.6f, 0.7f, 1.0f));
  fill_light->set_intensity(0.5f);
  scene->add_light(fill_light);

  // Point light (orange accent)
  auto point_light = scene::Light::create_point(
      glm::vec3(3.0f, 3.0f, 3.0f) * kWorldScale, 15.0f * kWorldScale,
      glm::vec3(1.0f, 0.7f, 0.4f));
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
    camera->set_aspect(engine.get_aspect_ratio() * 0.5f);
    renderer->resize(width, height);
    canvas->resize(width, height);
    std::cout << "Window resized: " << width << "x" << height
              << " (aspect: " << engine.get_aspect_ratio() << ")" << std::endl;
  });

  std::atomic<bool> leap_running(true);
  std::mutex leap_mutex;
  const LEAP_TRACKING_EVENT* latest_frame = nullptr;
  std::thread leap_thread([&]() {
    while (leap_running) {
        leap->update();
        std::lock_guard<std::mutex> lock(leap_mutex);
        latest_frame = leap->getCurFrame();
    }
  });

  static std::shared_ptr<scene::Node> held_disk = nullptr;
  static int held_by_hand = -1;
  static std::shared_ptr<scene::Node> held_original_parent = nullptr;
  static glm::vec3 held_original_local_position(0.0f);

  std::cout << "Scene root children: " << scene->get_root()->get_children().size() << std::endl;
for (auto& child : scene->get_root()->get_children()) {
    std::cout << "  " << child->get_name() << std::endl;
}

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
    float cam_speed = 5.0f * kWorldScale * dt;
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

    // render hands
    const LEAP_TRACKING_EVENT* frame = nullptr;
    {
        std::lock_guard<std::mutex> lock(leap_mutex);
        frame = latest_frame;
    }
    if (frame) {

      auto to_scene = [&](const LEAP_VECTOR& v) -> glm::vec3 {
        glm::vec3 leap_pos = (glm::vec3(v.x, v.y, v.z) / 10.0f) * kWorldScale;

        glm::vec3 forward = glm::normalize(camera->get_target() - camera_pos);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
        glm::vec3 up = glm::cross(right, forward);

        return camera_pos + right * leap_pos.x + up * (leap_pos.y - 20.0f * kWorldScale) +
               forward * (-leap_pos.z + 20.0f * kWorldScale);
      };

      for (int h = 0; h < 2; h++) {
        int base = h * JOINTS_PER_HAND;

        if (h >= (int)frame->nHands) {
            for (int j = 0; j < JOINTS_PER_HAND; j++)
                hand_nodes[base + j]->set_position(glm::vec3(0.0f, -1000.0f * kWorldScale, 0.0f));
            continue;
        }

        const LEAP_HAND& hand = frame->pHands[h];
        int idx = base;

        hand_nodes[idx++]->set_position(to_scene(hand.arm.prev_joint));  // elbow
        hand_nodes[idx++]->set_position(to_scene(hand.arm.next_joint));  // wrist
        hand_nodes[idx++]->set_position(to_scene(hand.palm.position));   // palm

        for (int f = 0; f < 5; f++)
            for (int b = 0; b < 4; b++)
                hand_nodes[idx++]->set_position(to_scene(hand.digits[f].bones[b].next_joint));
      }



    for (uint32_t h = 0; h < frame->nHands; h++) {
    const LEAP_HAND& hand = frame->pHands[h];
    glm::vec3 palm = to_scene(hand.palm.position);
    glm::vec3 index_tip = to_scene(hand.digits[1].bones[3].next_joint);

    // Try to grab
    if (hand.pinch_strength > 0.85f && held_disk == nullptr) {
      for (int i = (int)disks.size() - 1; i >= 0; i--) {
        auto& disk = disks[i];
        glm::vec3 disk_world_pos = glm::vec3(disk->get_world_transform()[3]);
        float dist = glm::length(index_tip - disk_world_pos);
        if (dist < 2.0f * kWorldScale) {
          const int hand_base = static_cast<int>(h) * JOINTS_PER_HAND;
          auto hand_palm_node = hand_nodes[hand_base + 2];

          held_original_parent = disk->get_parent();
          held_original_local_position = disk->get_position();

          glm::vec3 world_position = get_world_pos(disk);
          if (held_original_parent) {
            held_original_parent->remove_child(disk);
          }
          hand_palm_node->add_child(disk);
          glm::mat4 hand_world = hand_palm_node->get_world_transform();
          glm::vec3 local_pos = glm::vec3(glm::inverse(hand_world) * glm::vec4(world_position, 1.0f));
          disk->set_position(local_pos);

          held_disk = disk;
          held_by_hand = static_cast<int>(h);
          break;
        }
      }
    }

    // Update held disk position every frame
    if (held_disk && held_by_hand == (int)h) {
      auto parent = held_disk->get_parent();
      if (parent) {
        glm::mat4 parent_world = parent->get_world_transform();
        glm::mat4 world_to_local = glm::inverse(parent_world);
        glm::vec4 local_pos = world_to_local * glm::vec4(index_tip, 1.0f);
        held_disk->set_position(glm::vec3(local_pos));
      }
    }

    // Release
    if (hand.pinch_strength < 0.5f && held_by_hand == (int)h && held_disk) {
      glm::vec3 release_world_pos = index_tip;
      std::shared_ptr<scene::Node> closest_tower = nullptr;
      float closest_dist = std::numeric_limits<float>::max();
      for (const auto& tower : towers) {
        float dist = glm::length(release_world_pos - get_world_pos(tower));
        if (dist < closest_dist) {
          closest_dist = dist;
          closest_tower = tower;
        }
      }

      auto hand_parent = held_disk->get_parent();
      if (hand_parent) {
        hand_parent->remove_child(held_disk);
      }

      bool valid_move = (closest_tower != nullptr);
      std::shared_ptr<scene::Node> top_disk = nullptr;
      float top_y = -std::numeric_limits<float>::max();
      if (valid_move) {
        for (const auto& disk : disks) {
          if (disk == held_disk) {
            continue;
          }
          if (!is_descendant_of(disk, closest_tower)) {
            continue;
          }
          float y = get_world_pos(disk).y;
          if (!top_disk || y > top_y) {
            top_y = y;
            top_disk = disk;
          }
        }

        const int held_order = disk_order[held_disk.get()];
        const int top_order = top_disk ? disk_order[top_disk.get()] : std::numeric_limits<int>::max();
        if (top_disk && held_order < top_order) {
          valid_move = false;
        }
      }

      if (valid_move) {
        if (top_disk) {
          const glm::vec3 target_world = get_world_pos(top_disk) + glm::vec3(0.0f, disk_stack_step, 0.0f);
          top_disk->add_child(held_disk);
          const glm::mat4 top_world = top_disk->get_world_transform();
          const glm::vec3 local_target =
              glm::vec3(glm::inverse(top_world) * glm::vec4(target_world, 1.0f));
          held_disk->set_position(local_target);
        } else {
          glm::vec3 target_world = get_world_pos(closest_tower);
          target_world.y = tower_base_y;
          closest_tower->add_child(held_disk);
          glm::mat4 tower_world = closest_tower->get_world_transform();
          glm::vec3 local_target =
              glm::vec3(glm::inverse(tower_world) * glm::vec4(target_world, 1.0f));
          held_disk->set_position(local_target);
        }
      } else if (held_original_parent) {
        held_original_parent->add_child(held_disk);
        held_disk->set_position(held_original_local_position);
      }

      held_original_parent = nullptr;
      held_disk = nullptr;
      held_by_hand = -1;
    }
  }
}

    // Render scene with frustum culling (automatic)
    scene->update();
    auto eye_cameras = ovr_backend.build_stereo_cameras(*camera);
    renderer->render_stereo(scene, eye_cameras.left, eye_cameras.right);

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
  ovr_backend.shutdown();
  leap_running = false;
  leap_thread.join();
  engine.free();
  std::cout << "\nEngine shut down successfully." << std::endl;
  return 0;
}
