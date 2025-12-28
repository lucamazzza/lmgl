#include <gtest/gtest.h>

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"
#endif
#include "lmgl/renderer/framebuffer.hpp"

namespace lmgl {

namespace renderer {

class FramebufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto& engine_instance = core::Engine::get_instance();
        if (!engine_instance.get_window()) engine_instance.init(800, 600, "Framebuffer Test");
#endif
    }

    void TearDown() override {
        // Cleanup code after each test
    }
};

#ifndef TEST_HEADLESS

TEST_F(FramebufferTest, CreationAndCompleteness) {
    int width = 800;
    int height = 600;
    auto fbo = std::make_shared<Framebuffer>(width, height);
    EXPECT_NE(fbo, nullptr);
    auto attachment = fbo->get_color_attachment();
    EXPECT_NE(attachment, nullptr);
    EXPECT_GT(attachment->get_id(), 0);
    EXPECT_EQ(attachment->get_width(), width);
    EXPECT_EQ(attachment->get_height(), height);
}

TEST_F(FramebufferTest, ResizePropagationToAttachment) {
    auto fbo = std::make_shared<Framebuffer>(800, 600);
    auto attachment = fbo->get_color_attachment();
    EXPECT_EQ(attachment->get_width(), 800);
    EXPECT_EQ(attachment->get_height(), 600);
    fbo->resize(1024, 768);
    auto res_attachment = fbo->get_color_attachment();
    EXPECT_EQ(res_attachment->get_width(), 1024);
    EXPECT_EQ(res_attachment->get_height(), 768);
}

TEST_F(FramebufferTest, BindAndUnbind) {
    auto fbo = std::make_shared<Framebuffer>(800, 600);
    fbo->bind();
    fbo->unbind();
    SUCCEED();
}

TEST_F(FramebufferTest, MultipleFBOsDoNotConflict) {
    auto fboLeft = std::make_shared<Framebuffer>(512, 512);
    auto fboRight = std::make_shared<Framebuffer>(512, 512);
    auto texLeft = fboLeft->get_color_attachment();
    auto texRight = fboRight->get_color_attachment();
    EXPECT_NE(texLeft->get_id(), texRight->get_id());
    fboLeft->bind();
    fboRight->bind();
    fboLeft->unbind();
    fboRight->unbind();
}

#endif

} // namespace renderer

} // namespace lmgl
