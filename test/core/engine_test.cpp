#include <gtest/gtest.h>
#include "lmgl/core/engine.hpp"

namespace lmgl {

namespace core {

class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
    }

    void TearDown() override {
        // Cleanup code after each test
        Engine::get_instance().free();
    }

};

TEST_F(EngineTest, SingletonReturnsaSameInstance) {
    auto& engine1 = Engine::get_instance();
    auto& engine2 = Engine::get_instance();
    EXPECT_EQ(&engine1, &engine2);
}

TEST_F(EngineTest, InitializationSucceeds) {
    auto& engine = Engine::get_instance();
#ifndef TEST_HEADLESS
    bool result = engine.init(800, 600, "Test Window");
    EXPECT_TRUE(result);
    EXPECT_NE(engine.get_window(), nullptr);
#endif
}

TEST_F(EngineTest, DeltaTimeIsPositive) {
    auto& engine = Engine::get_instance();
    if (engine.init(800, 600, "Test Window")) {
        float dt = 0.0f;
        engine.run([&dt, &engine](float delta_time) {
            dt = delta_time;
            glfwSetWindowShouldClose(engine.get_window(), GLFW_TRUE);
        });
        EXPECT_GT(dt, 0.0f);
    }    
}

} // namespace core

} // namespace lmgl

