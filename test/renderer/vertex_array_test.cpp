#include <gtest/gtest.h>

#include "lmgl/core/engine.hpp"
#include "lmgl/renderer/vertex_array.hpp"

namespace lmgl {

namespace renderer {

class VertexArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test 
        auto& engine_instance = core::Engine::get_instance();
        if (!engine_instance.get_window()) engine_instance.init(800, 600, "VertexArray Test");
    }

    void TearDown() override {
        // Cleanup code after each test
    }
};

TEST_F(VertexArrayTest, Creation) {
    auto vao = std::make_shared<VertexArray>();
    EXPECT_NE(vao, nullptr);
}

TEST_F(VertexArrayTest, AddVertexBuffer) {
    float vertices[] = {
     -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
      0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    vbo->set_layout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float3, "a_Color" }
    });

    auto vao = std::make_shared<VertexArray>();
    vao->add_vertex_buffer(vbo);

    const auto& buffers = vao->get_vertex_buffers();
    EXPECT_EQ(buffers.size(), 1);
    EXPECT_EQ(buffers[0], vbo);
}

TEST_F(VertexArrayTest, SetIndexBuffer) {
    unsigned int indices[] = { 0, 1, 2 };
    auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));

    auto vao = std::make_shared<VertexArray>();
    vao->set_index_buffer(ibo);

    EXPECT_EQ(vao->get_index_buffer(), ibo);
    EXPECT_EQ(vao->get_index_buffer()->get_count(), 3);
}

TEST_F(VertexArrayTest, CompleteVertexArraySetup) {
    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    vbo->set_layout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    });
    auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
    auto vao = std::make_shared<VertexArray>();
    vao->add_vertex_buffer(vbo);
    vao->set_index_buffer(ibo);

    EXPECT_EQ(vao->get_vertex_buffers().size(), 1);
    EXPECT_EQ(vao->get_index_buffer()->get_count(), 6);

    // should not crash
    vao->bind();
    vao->unbind();
}

TEST_F(VertexArrayTest, MultipleVertexBuffers) {
    float positions[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    auto p_vbo = std::make_shared<VertexBuffer>(positions, sizeof(positions));
    p_vbo->set_layout({
        { ShaderDataType::Float3, "a_Position" }
    });
    float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    auto c_vbo = std::make_shared<VertexBuffer>(colors, sizeof(colors));
    c_vbo->set_layout({
        { ShaderDataType::Float4, "a_Color" }
    });
    auto vao = std::make_shared<VertexArray>();
    vao->add_vertex_buffer(p_vbo);
    vao->add_vertex_buffer(c_vbo);
    EXPECT_EQ(vao->get_vertex_buffers().size(), 2);
}

TEST_F(VertexArrayTest, BindUnbind) {
    auto vao = std::make_shared<VertexArray>();
    
    // should not crash
    vao->bind();
    vao->unbind();
}

} // namespace renderer

} // namespace lmgl
