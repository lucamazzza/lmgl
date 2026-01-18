#include "lmgl/scene/skybox.hpp"
#include <gtest/gtest.h>

using namespace lmgl::scene;

class SkyboxTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(SkyboxTest, CubemapFromFacesRequiresSixFaces) {
    std::vector<std::string> too_few_faces = {
        "face1.png", "face2.png", "face3.png"
    };
    auto cubemap = Cubemap::from_faces(too_few_faces);
    EXPECT_EQ(cubemap, nullptr);
}

TEST_F(SkyboxTest, CubemapFromEquirectangular) {
    auto cubemap = Cubemap::from_equirectangular("nonexistent.hdr");
    EXPECT_EQ(cubemap, nullptr);
}

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"

TEST_F(SkyboxTest, SkyboxConstruction) {
    auto& engine = lmgl::core::Engine::get_instance();
    if (!engine.get_window()) {
        engine.init(800, 600, "Skybox Test");
    }
    auto skybox = std::make_shared<Skybox>(nullptr);
    EXPECT_NE(skybox, nullptr);
}

TEST_F(SkyboxTest, SkyboxExposure) {
    auto& engine = lmgl::core::Engine::get_instance();
    if (!engine.get_window()) {
        engine.init(800, 600, "Skybox Test");
    }
    auto skybox = std::make_shared<Skybox>(nullptr);
    skybox->set_exposure(2.0f);
    EXPECT_FLOAT_EQ(skybox->get_exposure(), 2.0f);
    skybox->set_exposure(0.5f);
    EXPECT_FLOAT_EQ(skybox->get_exposure(), 0.5f);
}

#endif
