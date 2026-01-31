#include "lmgl/scene/lod.hpp"
#include "lmgl/scene/mesh.hpp"

#ifndef TEST_HEADLESS

#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/shader.hpp"

#endif

#include <gtest/gtest.h>
#include <memory>

namespace lmgl {

namespace scene {

class LODTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto &engine = core::Engine::get_instance();
        if (!engine.get_window())
            engine.init(800, 600, "LOD Test");
        shader = renderer::Shader::from_glsl_file("shaders/pbr.glsl");
#endif
    }

    void TearDown() override {
        // Cleanup code after each test
    }

#ifndef TEST_HEADLESS
    std::shared_ptr<renderer::Shader> shader;
#endif
};

// LODLevel Tests

TEST_F(LODTest, LODLevelConstruction) {
    std::vector<Vertex> vertices = {Vertex(glm::vec3(0.0f, 0.0f, 0.0f)), Vertex(glm::vec3(1.0f, 0.0f, 0.0f)),
                                    Vertex(glm::vec3(0.0f, 1.0f, 0.0f))};
    std::vector<unsigned int> indices = {0, 1, 2};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LODLevel level(mesh, 10.0f);

    EXPECT_EQ(level.mesh, mesh);
    EXPECT_FLOAT_EQ(level.max_distance_sq, 100.0f); // 10^2 = 100
#endif
}

TEST_F(LODTest, LODLevelDistanceSquared) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LODLevel level1(mesh, 5.0f);
    LODLevel level2(mesh, 20.0f);
    LODLevel level3(mesh, 100.0f);

    EXPECT_FLOAT_EQ(level1.max_distance_sq, 25.0f);
    EXPECT_FLOAT_EQ(level2.max_distance_sq, 400.0f);
    EXPECT_FLOAT_EQ(level3.max_distance_sq, 10000.0f);
#endif
}

// LOD Basic Tests

TEST_F(LODTest, DefaultConstruction) {
    LOD lod;

    EXPECT_EQ(lod.get_level_count(), 0);
    EXPECT_FALSE(lod.has_levels());
}

TEST_F(LODTest, AddLevel) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh, 10.0f);

    EXPECT_EQ(lod.get_level_count(), 1);
    EXPECT_TRUE(lod.has_levels());
    EXPECT_EQ(lod.get_level(0).mesh, mesh);
    EXPECT_FLOAT_EQ(lod.get_level(0).max_distance_sq, 100.0f);
#endif
}

TEST_F(LODTest, AddMultipleLevels) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh1 = std::make_shared<Mesh>(vertices, indices, shader);
    auto mesh2 = std::make_shared<Mesh>(vertices, indices, shader);
    auto mesh3 = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh1, 10.0f);
    lod.add_level(mesh2, 50.0f);
    lod.add_level(mesh3, 200.0f);

    EXPECT_EQ(lod.get_level_count(), 3);
    EXPECT_TRUE(lod.has_levels());

    EXPECT_EQ(lod.get_level(0).mesh, mesh1);
    EXPECT_EQ(lod.get_level(1).mesh, mesh2);
    EXPECT_EQ(lod.get_level(2).mesh, mesh3);
#endif
}

TEST_F(LODTest, AddNullMesh) {
    LOD lod;
    lod.add_level(nullptr, 10.0f);

    EXPECT_EQ(lod.get_level_count(), 0);
    EXPECT_FALSE(lod.has_levels());
}

TEST_F(LODTest, Clear) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh, 10.0f);
    lod.add_level(mesh, 50.0f);

    EXPECT_EQ(lod.get_level_count(), 2);

    lod.clear();

    EXPECT_EQ(lod.get_level_count(), 0);
    EXPECT_FALSE(lod.has_levels());
#endif
}

// LOD Mesh Selection Tests (Distance-based)

TEST_F(LODTest, GetMeshEmptyLOD) {
    LOD lod;

    auto mesh = lod.get_mesh(100.0f);
    EXPECT_EQ(mesh, nullptr);
}

TEST_F(LODTest, GetMeshSingleLevel) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh1 = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh1, 10.0f);

    // Within threshold
    EXPECT_EQ(lod.get_mesh(50.0f), mesh1); // sqrt(50) ≈ 7.07 < 10

    // At threshold
    EXPECT_EQ(lod.get_mesh(100.0f), mesh1); // sqrt(100) = 10

    // Beyond threshold - should still return last (only) level
    EXPECT_EQ(lod.get_mesh(500.0f), mesh1); // sqrt(500) ≈ 22.36 > 10
#endif
}

TEST_F(LODTest, GetMeshMultipleLevels) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto high_mesh = std::make_shared<Mesh>(vertices, indices, shader);
    auto med_mesh = std::make_shared<Mesh>(vertices, indices, shader);
    auto low_mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(high_mesh, 10.0f); // 0-10: high
    lod.add_level(med_mesh, 50.0f);  // 10-50: medium
    lod.add_level(low_mesh, 200.0f); // 50+: low

    // Test high detail range
    EXPECT_EQ(lod.get_mesh(0.0f), high_mesh);   // distance = 0
    EXPECT_EQ(lod.get_mesh(25.0f), high_mesh);  // distance = 5
    EXPECT_EQ(lod.get_mesh(100.0f), high_mesh); // distance = 10 (boundary)

    // Test medium detail range
    EXPECT_EQ(lod.get_mesh(121.0f), med_mesh);  // distance = 11
    EXPECT_EQ(lod.get_mesh(900.0f), med_mesh);  // distance = 30
    EXPECT_EQ(lod.get_mesh(2500.0f), med_mesh); // distance = 50 (boundary)

    // Test low detail range
    EXPECT_EQ(lod.get_mesh(2601.0f), low_mesh);  // distance = 51
    EXPECT_EQ(lod.get_mesh(10000.0f), low_mesh); // distance = 100
    EXPECT_EQ(lod.get_mesh(40000.0f), low_mesh); // distance = 200 (boundary)

    // Test beyond all thresholds
    EXPECT_EQ(lod.get_mesh(50000.0f), low_mesh);   // distance = 223.6
    EXPECT_EQ(lod.get_mesh(1000000.0f), low_mesh); // distance = 1000
#endif
}

TEST_F(LODTest, GetMeshWithPositions) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto high_mesh = std::make_shared<Mesh>(vertices, indices, shader);
    auto low_mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(high_mesh, 10.0f);
    lod.add_level(low_mesh, 100.0f);

    glm::vec3 object_pos(0.0f, 0.0f, 0.0f);

    // Camera close to object
    glm::vec3 camera_close(5.0f, 0.0f, 0.0f); // distance = 5
    EXPECT_EQ(lod.get_mesh(camera_close, object_pos), high_mesh);

    // Camera at boundary
    glm::vec3 camera_boundary(10.0f, 0.0f, 0.0f); // distance = 10
    EXPECT_EQ(lod.get_mesh(camera_boundary, object_pos), high_mesh);

    // Camera far from object
    glm::vec3 camera_far(50.0f, 0.0f, 0.0f); // distance = 50
    EXPECT_EQ(lod.get_mesh(camera_far, object_pos), low_mesh);

    // Camera very far
    glm::vec3 camera_very_far(200.0f, 0.0f, 0.0f); // distance = 200
    EXPECT_EQ(lod.get_mesh(camera_very_far, object_pos), low_mesh);
#endif
}

TEST_F(LODTest, GetMeshWith3DDistance) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto high_mesh = std::make_shared<Mesh>(vertices, indices, shader);
    auto low_mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(high_mesh, 10.0f);
    lod.add_level(low_mesh, 50.0f);

    glm::vec3 object_pos(0.0f, 0.0f, 0.0f);

    // 3D distance calculation: sqrt(3^2 + 4^2 + 0^2) = 5
    glm::vec3 camera1(3.0f, 4.0f, 0.0f);
    EXPECT_EQ(lod.get_mesh(camera1, object_pos), high_mesh);

    // 3D distance: sqrt(6^2 + 8^2 + 0^2) = 10
    glm::vec3 camera2(6.0f, 8.0f, 0.0f);
    EXPECT_EQ(lod.get_mesh(camera2, object_pos), high_mesh);

    // 3D distance: sqrt(10^2 + 20^2 + 20^2) = 30
    glm::vec3 camera3(10.0f, 20.0f, 20.0f);
    EXPECT_EQ(lod.get_mesh(camera3, object_pos), low_mesh);
#endif
}

// LOD Edge Cases

TEST_F(LODTest, ZeroDistance) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh, 10.0f);

    EXPECT_EQ(lod.get_mesh(0.0f), mesh);

    glm::vec3 pos(0.0f, 0.0f, 0.0f);
    EXPECT_EQ(lod.get_mesh(pos, pos), mesh);
#endif
}

TEST_F(LODTest, VerySmallDistance) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh, 0.1f);

    EXPECT_EQ(lod.get_mesh(0.0001f), mesh); // 0.01 < 0.1
    EXPECT_EQ(lod.get_mesh(0.01f), mesh);   // 0.1 = 0.1
#endif
}

TEST_F(LODTest, VeryLargeDistance) {
    std::vector<Vertex> vertices = {Vertex()};
    std::vector<unsigned int> indices = {0};

#ifndef TEST_HEADLESS
    auto mesh = std::make_shared<Mesh>(vertices, indices, shader);

    LOD lod;
    lod.add_level(mesh, 10000.0f);

    EXPECT_EQ(lod.get_mesh(100000000.0f), mesh);     // 10000 < 10000
    EXPECT_EQ(lod.get_mesh(1000000000000.0f), mesh); // Still returns last level
#endif
}

// Integration with Procedural Meshes

#ifndef TEST_HEADLESS

TEST_F(LODTest, ProceduralSphereLOD) {
    auto high_sphere = Mesh::create_sphere(shader, 1.0f, 64, 64);
    auto med_sphere = Mesh::create_sphere(shader, 1.0f, 32, 32);
    auto low_sphere = Mesh::create_sphere(shader, 1.0f, 16, 16);

    ASSERT_NE(high_sphere, nullptr);
    ASSERT_NE(med_sphere, nullptr);
    ASSERT_NE(low_sphere, nullptr);

    LOD lod;
    lod.add_level(high_sphere, 15.0f);
    lod.add_level(med_sphere, 50.0f);
    lod.add_level(low_sphere, 150.0f);

    EXPECT_EQ(lod.get_level_count(), 3);

    glm::vec3 object_pos(0.0f, 0.0f, 0.0f);

    EXPECT_EQ(lod.get_mesh(glm::vec3(5.0f, 0.0f, 0.0f), object_pos), high_sphere);
    EXPECT_EQ(lod.get_mesh(glm::vec3(30.0f, 0.0f, 0.0f), object_pos), med_sphere);
    EXPECT_EQ(lod.get_mesh(glm::vec3(100.0f, 0.0f, 0.0f), object_pos), low_sphere);
}

TEST_F(LODTest, ProceduralCubeLOD) {
    auto high_cube = Mesh::create_cube(shader, 4);
    auto med_cube = Mesh::create_cube(shader, 2);
    auto low_cube = Mesh::create_cube(shader, 1);

    ASSERT_NE(high_cube, nullptr);
    ASSERT_NE(med_cube, nullptr);
    ASSERT_NE(low_cube, nullptr);

    LOD lod;
    lod.add_level(high_cube, 10.0f);
    lod.add_level(med_cube, 30.0f);
    lod.add_level(low_cube, 100.0f);

    EXPECT_EQ(lod.get_level_count(), 3);
    EXPECT_TRUE(lod.has_levels());
}

#endif

} // namespace scene

} // namespace lmgl
