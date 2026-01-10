#include <gtest/gtest.h>

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"
#endif
#include "lmgl/renderer/texture.hpp"

#include <vector>

namespace lmgl {

namespace renderer {

class TextureTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto &engine_instance = core::Engine::get_instance();
        if (!engine_instance.get_window())
            engine_instance.init(800, 600, "Shader Test");
#endif
    }

    void TearDown() override {
        // Cleanup code after each test
    }
};

#ifndef TEST_HEADLESS

TEST_F(TextureTest, CreateEmptyTexture) {
    int width = 128;
    int height = 128;
    auto texture = std::make_shared<Texture>(width, height);
    EXPECT_NE(texture, nullptr);
    EXPECT_GT(texture->get_id(), 0);
    EXPECT_EQ(texture->get_width(), width);
    EXPECT_EQ(texture->get_height(), height);
}

TEST_F(TextureTest, ResizeTexture) {
    auto texture = std::make_shared<Texture>(128, 128);
    EXPECT_EQ(texture->get_width(), 128);
    EXPECT_EQ(texture->get_height(), 128);
    texture->resize(256, 256);
    EXPECT_EQ(texture->get_width(), 256);
    EXPECT_EQ(texture->get_height(), 256);
}

TEST_F(TextureTest, SetDataManual) {
    int width = 2;
    int height = 2;
    auto texture = std::make_shared<Texture>(width, height);
    std::vector<unsigned char> data(width * height * 4, 255);
    texture->set_data(data.data(), data.size());
    SUCCEED();
}

TEST_F(TextureTest, BindAndUnbind) {
    auto texture = std::make_shared<Texture>(32, 32);
    texture->bind(0);
    texture->bind(1);
    texture->unbind();
    SUCCEED();
}

TEST_F(TextureTest, LoadMissingFileSafely) {
    auto texture = std::make_shared<Texture>("non_existent_file.png");
    EXPECT_NE(texture, nullptr);
    EXPECT_EQ(texture->get_id(), 0);
}

#endif

} // namespace renderer

} // namespace lmgl
