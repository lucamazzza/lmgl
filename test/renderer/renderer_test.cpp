#include <gtest/gtest.h>

#ifndef TEST_HEADLESS

#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/renderer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/scene/camera.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"
#include "lmgl/scene/scene.hpp"

namespace lmgl {

namespace renderer {

class RendererTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
        auto &engine = core::Engine::get_instance();
        engine.init(800, 600, "Renderer Test");
        renderer = std::make_unique<Renderer>();
        scene = std::make_shared<scene::Scene>();
        camera = std::make_shared<scene::Camera>(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    }

    void TearDown() override {
        // Cleanup code after each test
    }

    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<scene::Scene> scene;
    std::shared_ptr<scene::Camera> camera;
};

TEST_F(RendererTest, ConstructorInitializesDefaults) {
    EXPECT_EQ(renderer->get_render_mode(), RenderMode::Solid);
    EXPECT_EQ(renderer->get_draw_calls(), 0);
    EXPECT_EQ(renderer->get_triangles_count(), 0);
}

TEST_F(RendererTest, SetRenderMode) {
    renderer->set_render_mode(RenderMode::Wireframe);
    EXPECT_EQ(renderer->get_render_mode(), RenderMode::Wireframe);
    renderer->set_render_mode(RenderMode::Points);
    EXPECT_EQ(renderer->get_render_mode(), RenderMode::Points);
    renderer->set_render_mode(RenderMode::Solid);
    EXPECT_EQ(renderer->get_render_mode(), RenderMode::Solid);
}

TEST_F(RendererTest, RenderNullSceneDoesNotCrash) { EXPECT_NO_THROW(renderer->render(nullptr, camera)); }

TEST_F(RendererTest, RenderNullCameraDoesNotCrash) { EXPECT_NO_THROW(renderer->render(scene, nullptr)); }

TEST_F(RendererTest, RenderEmptyScene) {
    renderer->render(scene, camera);
    EXPECT_EQ(renderer->get_draw_calls(), 0);
    EXPECT_EQ(renderer->get_triangles_count(), 0);
}

TEST_F(RendererTest, RenderSceneWithSingleMesh) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    scene->get_root()->add_child(node);
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    renderer->render(scene, camera);
    EXPECT_GT(renderer->get_draw_calls(), 0);
    EXPECT_GT(renderer->get_triangles_count(), 0);
}

TEST_F(RendererTest, RenderSceneWithMultipleMeshes) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    for (int i = 0; i < 5; ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        auto node = std::make_shared<scene::Node>("CubeNode" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(i * 2.0f, 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    camera->set_position(glm::vec3(5.0f, 0.0f, 15.0f));
    renderer->render(scene, camera);
    EXPECT_GT(renderer->get_draw_calls(), 0);
    EXPECT_GT(renderer->get_triangles_count(), 0);
}

TEST_F(RendererTest, StatsResetBetweenFrames) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    scene->get_root()->add_child(node);
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    renderer->render(scene, camera);
    int draw_calls_first = renderer->get_draw_calls();
    int triangles_first = renderer->get_triangles_count();
    renderer->render(scene, camera);
    int draw_calls_second = renderer->get_draw_calls();
    int triangles_second = renderer->get_triangles_count();
    EXPECT_EQ(draw_calls_first, draw_calls_second);
    EXPECT_EQ(triangles_first, triangles_second);
}

TEST_F(RendererTest, RenderModeChanges) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    scene->get_root()->add_child(node);
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    renderer->set_render_mode(RenderMode::Wireframe);
    EXPECT_NO_THROW(renderer->render(scene, camera));
    renderer->set_render_mode(RenderMode::Points);
    EXPECT_NO_THROW(renderer->render(scene, camera));
    renderer->set_render_mode(RenderMode::Solid);
    EXPECT_NO_THROW(renderer->render(scene, camera));
}

TEST_F(RendererTest, FrustumCullingCullsObjectsBehindCamera) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    node->set_position(glm::vec3(0.0f, 0.0f, 10.0f)); // Behind camera
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    EXPECT_EQ(renderer->get_draw_calls(), 0);
}

TEST_F(RendererTest, FrustumCullingRendersObjectsInView) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    node->set_position(glm::vec3(0.0f, 0.0f, 0.0f)); // In front of camera
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    EXPECT_GT(renderer->get_draw_calls(), 0);
}

TEST_F(RendererTest, FrustumCullingCullsObjectsBeyondFarPlane) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    node->set_position(glm::vec3(0.0f, 0.0f, -150.0f)); // Beyond far plane (100)
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    EXPECT_EQ(renderer->get_draw_calls(), 0);
}

TEST_F(RendererTest, FrustumCullingCullsObjectsOutsideSides) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto mesh = scene::Mesh::create_cube(shader);
    auto node = std::make_shared<scene::Node>("CubeNode");
    node->set_mesh(mesh);
    node->set_position(glm::vec3(100.0f, 0.0f, 0.0f)); // Far to the side
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    EXPECT_EQ(renderer->get_draw_calls(), 0);
}

TEST_F(RendererTest, FrustumCullingHandlesHierarchies) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    auto parent = std::make_shared<scene::Node>("Parent");
    parent->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    
    auto child1 = std::make_shared<scene::Node>("Child1");
    auto mesh1 = scene::Mesh::create_cube(shader);
    child1->set_mesh(mesh1);
    child1->set_position(glm::vec3(1.0f, 0.0f, 0.0f)); // Visible
    
    auto child2 = std::make_shared<scene::Node>("Child2");
    auto mesh2 = scene::Mesh::create_cube(shader);
    child2->set_mesh(mesh2);
    child2->set_position(glm::vec3(0.0f, 0.0f, 100.0f)); // Behind camera
    
    parent->add_child(child1);
    parent->add_child(child2);
    scene->get_root()->add_child(parent);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    // Only one child should be rendered
    EXPECT_EQ(renderer->get_draw_calls(), 1);
}

TEST_F(RendererTest, FrustumCullingRendersMultipleVisibleObjects) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    for (int i = 0; i < 5; ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i - 2), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    renderer->render(scene, camera);
    
    EXPECT_GT(renderer->get_draw_calls(), 0);
}

// Material Caching Tests

TEST_F(RendererTest, MaterialCachingWithSameMaterial) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto material = std::make_shared<scene::Material>("SharedMaterial");
    material->set_albedo(glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Create multiple meshes sharing the same material
    for (int i = 0; i < 5; ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        mesh->set_material(material);
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i * 2), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(5.0f, 0.0f, 15.0f));
    
    // Render should work correctly with shared material
    EXPECT_NO_THROW(renderer->render(scene, camera));
    EXPECT_EQ(renderer->get_draw_calls(), 5);
}

TEST_F(RendererTest, MaterialCachingWithDifferentMaterials) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    // Create meshes with different materials
    for (int i = 0; i < 5; ++i) {
        auto material = std::make_shared<scene::Material>("Material" + std::to_string(i));
        material->set_albedo(glm::vec3(float(i) * 0.2f, 0.5f, 1.0f));
        material->set_metallic(float(i) * 0.2f);
        material->set_roughness(0.5f);
        
        auto mesh = scene::Mesh::create_cube(shader);
        mesh->set_material(material);
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i * 2), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(5.0f, 0.0f, 15.0f));
    
    EXPECT_NO_THROW(renderer->render(scene, camera));
    EXPECT_EQ(renderer->get_draw_calls(), 5);
}

TEST_F(RendererTest, MaterialCachingWithMixedMaterials) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto material1 = std::make_shared<scene::Material>("Material1");
    material1->set_albedo(glm::vec3(1.0f, 0.0f, 0.0f));
    
    auto material2 = std::make_shared<scene::Material>("Material2");
    material2->set_albedo(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Create meshes alternating between two materials
    for (int i = 0; i < 10; ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        mesh->set_material(i % 2 == 0 ? material1 : material2);
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i * 2 - 9), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 25.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    EXPECT_NO_THROW(renderer->render(scene, camera));
    EXPECT_GT(renderer->get_draw_calls(), 0);
}

TEST_F(RendererTest, MaterialCachingWithNullMaterials) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    // Create meshes without materials (should use default)
    for (int i = 0; i < 3; ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        // No material set - will use default
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i * 2), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(3.0f, 0.0f, 10.0f));
    
    // Should use default material for all and cache it
    EXPECT_NO_THROW(renderer->render(scene, camera));
    EXPECT_EQ(renderer->get_draw_calls(), 3);
}

TEST_F(RendererTest, MaterialCachingClearsBetweenFrames) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    auto material = std::make_shared<scene::Material>("TestMaterial");
    
    auto mesh = scene::Mesh::create_cube(shader);
    mesh->set_material(material);
    auto node = std::make_shared<scene::Node>("Cube");
    node->set_mesh(mesh);
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    
    // Render first frame
    renderer->render(scene, camera);
    EXPECT_EQ(renderer->get_draw_calls(), 1);
    
    // Render second frame - cache should be cleared at start
    renderer->render(scene, camera);
    EXPECT_EQ(renderer->get_draw_calls(), 1);
}

TEST_F(RendererTest, MaterialCachingSortsByMaterial) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    // Create materials
    auto materialA = std::make_shared<scene::Material>("MaterialA");
    materialA->set_albedo(glm::vec3(1.0f, 0.0f, 0.0f));
    
    auto materialB = std::make_shared<scene::Material>("MaterialB");
    materialB->set_albedo(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Add meshes in mixed order: A, B, A, B, A
    // After sorting, should be grouped: A, A, A, B, B
    std::vector<std::shared_ptr<scene::Material>> materials = {
        materialA, materialB, materialA, materialB, materialA
    };
    
    for (size_t i = 0; i < materials.size(); ++i) {
        auto mesh = scene::Mesh::create_cube(shader);
        mesh->set_material(materials[i]);
        auto node = std::make_shared<scene::Node>("Cube" + std::to_string(i));
        node->set_mesh(mesh);
        node->set_position(glm::vec3(float(i * 2), 0.0f, 0.0f));
        scene->get_root()->add_child(node);
    }
    
    camera->set_position(glm::vec3(5.0f, 0.0f, 15.0f));
    
    // Should render all meshes, sorted by material
    EXPECT_NO_THROW(renderer->render(scene, camera));
    EXPECT_EQ(renderer->get_draw_calls(), 5);
}

TEST_F(RendererTest, MaterialCachingHandlesMaterialChangeBetweenFrames) {
    auto shader = Shader::from_glsl_file("basic.glsl");
    
    auto material1 = std::make_shared<scene::Material>("Material1");
    auto material2 = std::make_shared<scene::Material>("Material2");
    
    auto mesh = scene::Mesh::create_cube(shader);
    mesh->set_material(material1);
    auto node = std::make_shared<scene::Node>("Cube");
    node->set_mesh(mesh);
    scene->get_root()->add_child(node);
    
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    
    // Render with first material
    renderer->render(scene, camera);
    EXPECT_EQ(renderer->get_draw_calls(), 1);
    
    // Change material
    mesh->set_material(material2);
    
    // Render with second material
    renderer->render(scene, camera);
    EXPECT_EQ(renderer->get_draw_calls(), 1);
}

} // namespace renderer

} // namespace lmgl

#endif
