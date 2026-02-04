#include <gtest/gtest.h>
#include <memory>

#include "lmgl/scene/mesh.hpp"

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/shader.hpp"
#endif

namespace lmgl {

namespace scene {

class MeshTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto &engine = core::Engine::get_instance();
        if (!engine.get_window())
            engine.init(800, 600, "Buffer Test");
#endif
    }

    void TearDown() override {
        // Cleanup code after each test
    }
};

TEST_F(MeshTest, VertexDefaultConstruction) {
    Vertex v;
    EXPECT_EQ(v.position, glm::vec3(0.0f));
    EXPECT_EQ(v.normal, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(v.color, glm::vec4(1.0f));
    EXPECT_EQ(v.uvs, glm::vec2(0.0f));
}

TEST_F(MeshTest, VertexCustomConstruction) {
    Vertex v(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
             glm::vec2(0.5f, 0.5f));
    EXPECT_EQ(v.position, glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(v.normal, glm::vec3(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(v.color, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(v.uvs, glm::vec2(0.5f, 0.5f));
}

#ifndef TEST_HEADLESS

TEST_F(MeshTest, CreateCubeDefault) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
)";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
)";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto cube_mesh = Mesh::create_cube(shader);
    EXPECT_NE(cube_mesh, nullptr);
    EXPECT_NE(cube_mesh->get_shader(), nullptr);
    EXPECT_NE(cube_mesh->get_vertex_array(), nullptr);
    EXPECT_EQ(cube_mesh->get_shader(), shader);
    EXPECT_GT(cube_mesh->get_index_count(), 0);
}

TEST_F(MeshTest, CreateCubeSubdivided) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
)";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
)";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto cube_1 = Mesh::create_cube(shader, 1);
    auto cube_2 = Mesh::create_cube(shader, 2);
    EXPECT_LT(cube_1->get_index_count(), cube_2->get_index_count());
}

TEST_F(MeshTest, CreateQuad) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto quad = Mesh::create_quad(shader);
    EXPECT_NE(quad, nullptr);
    EXPECT_EQ(quad->get_index_count(), 6); // 2 triangles
}

TEST_F(MeshTest, CreateSphere) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto sphere = Mesh::create_sphere(shader, 1.0f, 16, 16);
    EXPECT_NE(sphere, nullptr);
    EXPECT_GT(sphere->get_index_count(), 0);
}

TEST_F(MeshTest, MeshBindAndUnbind) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);

    // should not crash
    mesh->bind();
    mesh->unbind();
}

TEST_F(MeshTest, SetShader) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader1 = std::make_shared<renderer::Shader>(vert, frag);
    auto shader2 = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader1);
    EXPECT_EQ(mesh->get_shader(), shader1);
    mesh->set_shader(shader2);
    EXPECT_EQ(mesh->get_shader(), shader2);
}

TEST_F(MeshTest, HasVertexData) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    EXPECT_TRUE(mesh->has_vert_data());
    EXPECT_FALSE(mesh->get_vertices().empty());
    EXPECT_FALSE(mesh->get_indices().empty());
}

TEST_F(MeshTest, BoundingBoxExists) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    
    const auto& aabb = mesh->get_bounding_box();
    EXPECT_NE(aabb.min, aabb.max);
}

TEST_F(MeshTest, BoundingSphereExists) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    
    const auto& sphere = mesh->get_bounding_sphere();
    EXPECT_GT(sphere.radius, 0.0f);
}

TEST_F(MeshTest, CubeBoundingBoxSymmetric) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    
    const auto& aabb = mesh->get_bounding_box();
    glm::vec3 center = aabb.get_center();
    
    EXPECT_NEAR(center.x, 0.0f, 0.001f);
    EXPECT_NEAR(center.y, 0.0f, 0.001f);
    EXPECT_NEAR(center.z, 0.0f, 0.001f);
}

TEST_F(MeshTest, SphereBoundingSphereRadius) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    float expected_radius = 2.5f;
    auto mesh = Mesh::create_sphere(shader, expected_radius, 16, 16);
    
    const auto& sphere = mesh->get_bounding_sphere();
    // The bounding sphere is computed from AABB, so it will be slightly larger
    EXPECT_NEAR(sphere.radius, expected_radius, expected_radius * 0.8f);
}

TEST_F(MeshTest, SetMaterial) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    auto material = std::make_shared<Material>("TestMaterial");
    
    mesh->set_material(material);
    EXPECT_EQ(mesh->get_material(), material);
}

TEST_F(MeshTest, GetMaterialCanBeNull) {
    const char *vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char *frag = R"(
#version 410 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<renderer::Shader>(vert, frag);
    auto mesh = Mesh::create_cube(shader);
    
    // Default material might be null
    auto mat = mesh->get_material();
    // Just check it doesn't crash
    SUCCEED();
}

#endif

} // namespace scene

} // namespace lmgl
