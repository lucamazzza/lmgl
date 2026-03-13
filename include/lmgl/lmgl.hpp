/*!
 * @file lmgl.hpp
 * @brief Master include header for LMGL - Include this for most common functionality.
 *
 * This header provides convenient access to the most commonly used LMGL classes
 * and functions. For specialized features (VR, advanced rendering), include
 * specific headers as needed.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

// Core
#include "lmgl/core/engine.hpp"
#include "lmgl/input.hpp"

// Scene
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/light.hpp"
#include "lmgl/scene/material.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"
#include "lmgl/scene/skybox.hpp"

// Assets
#include "lmgl/assets/model_loader.hpp"
#include "lmgl/assets/texture_library.hpp"

// Renderer (commonly used)
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"

// UI
#include "lmgl/ui/button.hpp"
#include "lmgl/ui/canvas.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/slider.hpp"
#include "lmgl/ui/text.hpp"
#include "lmgl/ui/toggle.hpp"
#include "lmgl/ui/ui_loader.hpp"

// Namespace aliases for convenience
namespace lmgl {

// Core types
using Engine = core::Engine;
using Key = core::Key;

// Scene types
using Scene = scene::Scene;
using Camera = scene::Camera;
using Mesh = scene::Mesh;
using Node = scene::Node;
using Light = scene::Light;
using Material = scene::Material;
using Skybox = scene::Skybox;

// Asset types
using ModelLoader = assets::ModelLoader;
using TextureLibrary = assets::TextureLibrary;

// Renderer types
using Shader = renderer::Shader;
using Texture = renderer::Texture;

// UI namespace remains as-is (ui::)
// This allows: lmgl::ui::Button, lmgl::ui::Canvas, etc.

} // namespace lmgl

/*!
 * @mainpage LMGL
 *
 * @section intro_sec Introduction
 * LMGL is a modern C++ graphics library built on OpenGL, providing:
 * - PBR rendering with shadow mapping
 * - Scene graph with frustum culling
 * - Asset loading (models, textures)
 * - 2D UI system with JSON-based loading
 * - Easy-to-use API
 *
 * @section quick_start Quick Start
 * @code
 * #include <lmgl/lmgl.hpp>
 *
 * int main() {
 *     using namespace lmgl;
 *
 *     auto& engine = Engine::get_instance();
 *     if (!engine.init(1280, 720, "My Game")) {
 *         return -1;
 *     }
 *
 *     auto scene = std::make_shared<Scene>();
 *     auto camera = std::make_shared<Camera>();
 *
 *     engine.run([&](float dt) {
 *         scene->render(camera);
 *     });
 *
 *     engine.free();
 *     return 0;
 * }
 * @endcode
 *
 * @section modules Modules
 * - @ref core "Core" - Engine and lifecycle management
 * - @ref scene "Scene" - 3D scene graph and rendering
 * - @ref assets "Assets" - Model and texture loading
 * - @ref ui "UI" - 2D user interface system
 * - @ref renderer "Renderer" - Low-level rendering primitives
 */
