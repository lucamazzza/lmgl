#include "lmgl/renderer/shadow_map.hpp"
#include <gtest/gtest.h>

using namespace lmgl::renderer;

#ifndef TEST_HEADLESS

// Shadow map tests require OpenGL context
#include "lmgl/core/engine.hpp"

class ShadowMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& engine = lmgl::core::Engine::get_instance();
        if (!engine.get_window()) {
            engine.init(800, 600, "Shadow Test");
        }
    }
};

TEST_F(ShadowMapTest, ShadowMapConstruction) {
    EXPECT_NO_THROW({
        ShadowMap shadow_map;
    });
}

TEST_F(ShadowMapTest, ShadowMapCustomSize) {
    EXPECT_NO_THROW({
        ShadowMap shadow_map(1024, 1024);
    });
}

TEST_F(ShadowMapTest, ShadowMapGetDimensions) {
    ShadowMap shadow_map(512, 512);
    EXPECT_EQ(shadow_map.get_width(), 512u);
    EXPECT_EQ(shadow_map.get_height(), 512u);
}

TEST_F(ShadowMapTest, CubemapShadowMapConstruction) {
    EXPECT_NO_THROW({
        CubemapShadowMap cubemap_shadow;
    });
}

TEST_F(ShadowMapTest, CubemapShadowMapCustomResolution) {
    EXPECT_NO_THROW({
        CubemapShadowMap cubemap_shadow(512);
    });
}

TEST_F(ShadowMapTest, CubemapShadowMapGetResolution) {
    CubemapShadowMap cubemap_shadow(256);
    EXPECT_EQ(cubemap_shadow.get_resolution(), 256u);
}

TEST_F(ShadowMapTest, ShadowRendererConstruction) {
    EXPECT_NO_THROW({
        ShadowRenderer renderer;
    });
}

#endif
