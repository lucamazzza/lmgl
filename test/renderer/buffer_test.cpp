#include <gtest/gtest.h>

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"
#endif
#include "lmgl/renderer/buffer.hpp"

namespace lmgl {

namespace renderer {

class BufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto& engine = core::Engine::get_instance();
        if (!engine.get_window()) engine.init(800, 600, "Buffer Test");
#endif
    }
    
    void TearDown() override {
        // Cleanup code after each test
    }
};

TEST_F(BufferTest, ShaderDataTypeSizeCalculation) {
    BufferElement elem_float(ShaderDataType::Float, "test");
    EXPECT_EQ(elem_float.size, 4);

    BufferElement elem_float2(ShaderDataType::Float2, "test");
    EXPECT_EQ(elem_float2.size, 8);

    BufferElement elem_float3(ShaderDataType::Float3, "test");
    EXPECT_EQ(elem_float3.size, 12);

    BufferElement elem_float4(ShaderDataType::Float4, "test");
    EXPECT_EQ(elem_float4.size, 16);

    BufferElement elem_mat3(ShaderDataType::Mat3, "test");
    EXPECT_EQ(elem_mat3.size, 36);

    BufferElement elem_mat4(ShaderDataType::Mat4, "test");
    EXPECT_EQ(elem_mat4.size, 64);
}

TEST_F(BufferTest, ComponentCountCorrect) {
    BufferElement elem_float(ShaderDataType::Float, "test");
    EXPECT_EQ(elem_float.get_component_count(), 1);

    BufferElement elem_float2(ShaderDataType::Float2, "test");
    EXPECT_EQ(elem_float2.get_component_count(), 2);

    BufferElement elem_float3(ShaderDataType::Float3, "test");
    EXPECT_EQ(elem_float3.get_component_count(), 3);

    BufferElement elem_float4(ShaderDataType::Float4, "test");
    EXPECT_EQ(elem_float4.get_component_count(), 4);

    BufferElement elem_mat3(ShaderDataType::Mat3, "test");
    EXPECT_EQ(elem_mat3.get_component_count(), 9);

    BufferElement elem_mat4(ShaderDataType::Mat4, "test");
    EXPECT_EQ(elem_mat4.get_component_count(), 16);
}

TEST_F(BufferTest, BufferLayoutStrideCalculation) {
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    };

    // Stride should be 3*4 + 4*4 = 28
    EXPECT_EQ(layout.get_stride(), 28);
}

TEST_F(BufferTest, BufferLayoutOffsetCalculation) {
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
        { ShaderDataType::Float2, "a_TexCoord" }
    };

    const auto& elements = layout.get_elements();
    EXPECT_EQ(elements.size(), 3);

    EXPECT_EQ(elements[0].offset, 0);          // Float3 starts at 0
    EXPECT_EQ(elements[1].offset, 12);         // Float4 starts at 12
    EXPECT_EQ(elements[2].offset, 28);         // Float2 starts at 28
}

#ifndef TEST_HEADLESS
TEST_F(BufferTest, VertexBufferCalculation) {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    EXPECT_NE(vbo, nullptr);
    
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" }
    };
    vbo->set_layout(layout);
    EXPECT_EQ(vbo->get_layout().get_stride(), 12);
}

TEST_F(BufferTest, DynamicVertexBufferCreation) {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    EXPECT_NE(vbo, nullptr);

    float new_vertices[] = {
        -0.6f, -0.6f, 0.0f,
         0.6f, -0.6f, 0.0f,
         0.0f,  0.6f, 0.0f
    };
    vbo->set_data(new_vertices, sizeof(new_vertices));
}

TEST_F(BufferTest, IndexBufferCreation) {
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
    EXPECT_NE(ibo, nullptr);
    EXPECT_EQ(ibo->get_count(), 6);
}

TEST_F(BufferTest, BufferBindUnbind) {
    float vertices[] = { -0.5f, -0.5f, 0.0f };
    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));

    // should not crash
    vbo->bind();
    vbo->unbind();

    unsigned int indices[] = { 0, 1, 2 };
    auto ibo = std::make_shared<IndexBuffer>(indices, 3);
    
    // should not crash
    ibo->bind();
    ibo->unbind();
}

#endif 

} // namespace renderer

} // namespace lmgl
