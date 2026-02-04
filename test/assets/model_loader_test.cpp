#include "lmgl/assets/model_loader.hpp"
#include <gtest/gtest.h>

using namespace lmgl::assets;

class ModelLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Model loader tests are API-only, no OpenGL needed
    }
};

TEST_F(ModelLoaderTest, ModelLoadOptionsDefaults) {
    ModelLoadOptions options;
    EXPECT_TRUE(options.flip_uvs);
    EXPECT_TRUE(options.compute_tangents);
    EXPECT_TRUE(options.optimize_meshes);
    EXPECT_TRUE(options.triangulate);
    EXPECT_FLOAT_EQ(options.scale, 1.0f);
}

TEST_F(ModelLoaderTest, ModelLoadOptionsCustom) {
    ModelLoadOptions options;
    options.flip_uvs = false;
    options.compute_tangents = false;
    options.scale = 0.5f;
    EXPECT_FALSE(options.flip_uvs);
    EXPECT_FALSE(options.compute_tangents);
    EXPECT_FLOAT_EQ(options.scale, 0.5f);
}

TEST_F(ModelLoaderTest, LoadOptionsScaling) {
    ModelLoadOptions options;
    options.scale = 2.0f;
    EXPECT_FLOAT_EQ(options.scale, 2.0f);
}

TEST_F(ModelLoaderTest, ModelLoadOptionsAllFlags) {
    ModelLoadOptions options;
    options.flip_uvs = false;
    options.compute_tangents = false;
    options.optimize_meshes = false;
    options.triangulate = false;
    options.scale = 0.1f;
    EXPECT_FALSE(options.flip_uvs);
    EXPECT_FALSE(options.compute_tangents);
    EXPECT_FALSE(options.optimize_meshes);
    EXPECT_FALSE(options.triangulate);
    EXPECT_FLOAT_EQ(options.scale, 0.1f);
}

#ifndef TEST_HEADLESS
#include "lmgl/renderer/shader.hpp"
#include "lmgl/core/engine.hpp"

TEST_F(ModelLoaderTest, LoadNonExistentModel) {
    auto& engine = lmgl::core::Engine::get_instance();
    if (!engine.get_window()) {
        engine.init(800, 600, "Test");
    }
    const char* vert = R"(
        #version 410 core
        layout(location = 0) in vec3 a_Position;
        void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char* frag = R"(
        #version 410 core
        out vec4 FragColor;
        void main() { FragColor = vec4(1.0); }
    )";
    auto shader = std::make_shared<lmgl::renderer::Shader>(vert, frag);
    ModelLoadOptions options;
    auto node = ModelLoader::load("nonexistent_model.obj", shader, options);
    EXPECT_EQ(node, nullptr);
}

TEST_F(ModelLoaderTest, LoadWithNullShader) {
    ModelLoadOptions options;
    auto node = ModelLoader::load("model.obj", nullptr, options);
    EXPECT_EQ(node, nullptr);
}

#endif
