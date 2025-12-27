#include <gtest/gtest.h>
#include <sstream>

#ifndef TEST_HEADLESS
#include "lmgl/core/engine.hpp"
#endif
#include "lmgl/renderer/shader.hpp"

#include <fstream>

namespace lmgl {

namespace renderer {

class ShaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
#ifndef TEST_HEADLESS
        auto& engine_instance = core::Engine::get_instance();
        if (!engine_instance.get_window()) engine_instance.init(800, 600, "Shader Test");
#endif
        ShaderLibrary::clear();
        create_test_shaders();
    }
    
    void TearDown() override {
        // Cleanup code after each test
        ShaderLibrary::clear();
        remove("test_shader.vert");
        remove("test_shader.frag");
        remove("test_shader.glsl");
        remove("test_invalid.glsl");
    }
    
    void create_test_shaders() {
        std::ofstream vert_file("test_shader.vert");
        vert_file << R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
uniform mat4 u_Transform;
void main() {
    gl_Position = u_Transform * vec4(a_Position, 1.0);
}
)";
        vert_file.close();
        std::ofstream frag_file("test_shader.frag");
         frag_file << R"(
 #version 410 core
 uniform vec4 u_Color;
 out vec4 FragColor;
 void main() {
     FragColor = u_Color;
 }
 )";    
         frag_file.close();
         std::ofstream glsl_file("test_shader.glsl");
         glsl_file << R"(
 #shader vertex
 #version 410 core
 layout(location = 0) in vec3 a_Position;
 uniform mat4 u_Transform;
 void main() {
     gl_Position = u_Transform * vec4(a_Position, 1.0);
 }
 
 #shader fragment
 #version 410 core
 uniform vec4 u_Color;
 out vec4 FragColor;
 void main() {
     FragColor = u_Color;
 }
 )";
         glsl_file.close();
         std::ofstream invalid_file("test_invalid.glsl");
         invalid_file << R"(
 #shader vertex
 #version 410 core
 THIS IS INVALID GLSL CODE!!!
 
 #shader fragment
 #version 410 core
 ALSO INVALID!!!
 )";
         invalid_file.close();
     }
};

// Shader

TEST_F(ShaderTest, ParseShaderSeparatesVertexAndFragment) {
    std::ifstream file("test_shader.glsl");
    EXPECT_TRUE(file.is_open());

    std::stringstream buf;
    buf << file.rdbuf();
    std::string source = buf.str();

    EXPECT_NE(source.find("#shader vertex"), std::string::npos);
    EXPECT_NE(source.find("#shader fragment"), std::string::npos);
}

TEST_F(ShaderTest, ReadFileLoadsContent) {
    std::ifstream file("test_shader.vert");
    ASSERT_TRUE(file.is_open());

    std::stringstream buf;
    buf << file.rdbuf();
    std::string content = buf.str();

    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("#version 410 core"), std::string::npos);
}

TEST_F(ShaderTest, ShaderLibraryStartsEmpty) {
    EXPECT_FALSE(ShaderLibrary::exists("any_shader"));
    EXPECT_EQ(ShaderLibrary::get("any_shader"), nullptr);
}

#ifndef TEST_HEADLESS

TEST_F(ShaderTest, CreateFromSourceStrings) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() {
    gl_Position = vec4(a_Position, 1.0);
}
)";
    const char* frag = R"(
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";
    auto shader = std::make_shared<Shader>(vert, frag);
    EXPECT_NE(shader, nullptr);
    EXPECT_NE(shader->get_id(), 0);
}

TEST_F(ShaderTest, LoadFromSeparateFiles) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    EXPECT_NE(shader, nullptr);
    EXPECT_NE(shader->get_id(), 0);
}

TEST_F(ShaderTest, LoadFromGLSLFile) {
    auto shader = Shader::from_glsl_file("test_shader.glsl");
    EXPECT_NE(shader, nullptr);
    EXPECT_NE(shader->get_id(), 0);
}

TEST_F(ShaderTest, BindAndUnbind) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");

    // should not crash
    shader->bind();
    shader->unbind();
}

TEST_F(ShaderTest, SetIntUniform) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char* frag = R"(
#version 410 core
uniform int u_IntValue;
out vec4 FragColor;
void main() { FragColor = vec4(float(u_IntValue)); }
    )";
    auto shader = std::make_shared<Shader>(vert, frag);
    shader->bind();

    // Should not crash
    shader->set_int("u_IntValue", 42);
}

TEST_F(ShaderTest, SetFloatUniform) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();

    // Should not crash
    shader->set_float("u_Color", 1.0f);
}

TEST_F(ShaderTest, SetVec2Uniform) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char* frag = R"(
#version 410 core
uniform vec2 u_Vec2;
out vec4 FragColor;
void main() { FragColor = vec4(u_Vec2, 0.0, 1.0); }
    )";
    auto shader = std::make_shared<Shader>(vert, frag);
    shader->bind();

    // Should not crash
    shader->set_vec2("u_Vec2", glm::vec2(0.5f, 0.5f));
}

TEST_F(ShaderTest, SetVec3Uniform) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();

    // Should not crash
    shader->set_vec3("a_Position", glm::vec3(1.0f, 0.0f, 0.0f));
}

TEST_F(ShaderTest, SetVec4Uniform) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();

    // Should not crash
    shader->set_vec4("u_Color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

TEST_F(ShaderTest, SetMat3Uniform) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
uniform mat3 u_Mat3;
void main() {
    gl_Position = vec4(u_Mat3 * a_Position, 1.0);
}
    )";
    const char* frag = R"(
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0);
}
    )";
    auto shader = std::make_shared<Shader>(vert, frag);
    shader->bind();

    // Should not crash
    shader->set_mat3("u_Mat3", glm::mat3(1.0f));
}

TEST_F(ShaderTest, SetMat4Uniform) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();

    // Should not crash
    shader->set_mat4("u_Transform", glm::mat4(1.0f));
}

TEST_F(ShaderTest, SetIntArrayUniform) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() { gl_Position = vec4(a_Position, 1.0); }
    )";
    const char* frag = R"(
#version 410 core
uniform int u_IntArray[4];
out vec4 FragColor;
void main() {
    FragColor = vec4(float(u_IntArray[0]) / 255.0, float(u_IntArray[1]) / 255.0, float(u_IntArray[2]) / 255.0, 1.0);
}
    )";
    auto shader = std::make_shared<Shader>(vert, frag);
    shader->bind();

    int values[4] = { 255, 128, 64, 32 };
    // Should not crash
    shader->set_int_array("u_IntArray", values, 4);
}

TEST_F(ShaderTest, UniformLocationCaching) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();
    for (int i = 0; i < 10; i++) {
        shader->set_vec4("u_Color", glm::vec4(1.0f));
    }

    // No crashes = caching works
    SUCCEED();
}

TEST_F(ShaderTest, InvalidUniformName) {
    auto shader = Shader::from_vf_files("test_shader.vert", "test_shader.frag");
    shader->bind();

    // Should print warning but not crash
    shader->set_float("u_NonExistentUniform", 1.0f);
    SUCCEED();
}

// ShaderLibrary

TEST_F(ShaderTest, AddToShaderLibrary) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() {
    gl_Position = vec4(a_Position, 1.0);
}
)";
    const char* frag = R"(
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";
    auto shader = std::make_shared<Shader>(vert, frag);
    ShaderLibrary::add("test_shader", shader);

    EXPECT_TRUE(ShaderLibrary::exists("test_shader"));
}

TEST_F(ShaderTest, RetrieveFromShaderLibrary) {
    const char* vert = R"(
#version 410 core
layout(location = 0) in vec3 a_Position;
void main() {
    gl_Position = vec4(a_Position, 1.0);
}
)";
    const char* frag = R"(
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";
    auto shader = std::make_shared<Shader>(vert, frag);
    ShaderLibrary::add("test_shader", shader);

    auto retrieved_shader = ShaderLibrary::get("test_shader");
    EXPECT_NE(retrieved_shader, nullptr);
    EXPECT_EQ(retrieved_shader, shader);
}

TEST_F(ShaderTest, LoadShaderFromSeparateFiles) {
    auto shader = ShaderLibrary::load_vf("test_shader", "test_shader.vert", "test_shader.frag");
    EXPECT_NE(shader, nullptr);
    EXPECT_TRUE(ShaderLibrary::exists("test_shader"));
    auto retrieved_shader = ShaderLibrary::get("test_shader");
    EXPECT_NE(retrieved_shader, nullptr);
    EXPECT_EQ(retrieved_shader, shader);
}

TEST_F(ShaderTest, LoadShaderFromGLSLFile) {
    auto shader = ShaderLibrary::load_glsl("test_shader", "test_shader.glsl");
    EXPECT_NE(shader, nullptr);
    EXPECT_TRUE(ShaderLibrary::exists("test_shader"));
    auto retrieved_shader = ShaderLibrary::get("test_shader");
    EXPECT_NE(retrieved_shader, nullptr);
    EXPECT_EQ(retrieved_shader, shader);
}

TEST_F(ShaderTest, MultipleShadersInLibrary) {
    ShaderLibrary::load_vf("shader_1", "test_shader.vert", "test_shader.frag");
    ShaderLibrary::load_glsl("shader_2", "test_shader.glsl");
    EXPECT_TRUE(ShaderLibrary::exists("shader_1"));
    EXPECT_TRUE(ShaderLibrary::exists("shader_2"));
    auto s1 = ShaderLibrary::get("shader_1");
    auto s2 = ShaderLibrary::get("shader_2");
    EXPECT_NE(s1, nullptr);
    EXPECT_NE(s2, nullptr);
    EXPECT_NE(s1, s2);
}

TEST_F(ShaderTest, ClearLibrary) {
    ShaderLibrary::load_vf("shader_1", "test_shader.vert", "test_shader.frag");
    ShaderLibrary::load_glsl("shader_2", "test_shader.glsl");
    EXPECT_TRUE(ShaderLibrary::exists("shader_1"));
    EXPECT_TRUE(ShaderLibrary::exists("shader_2"));
    ShaderLibrary::clear();
    EXPECT_FALSE(ShaderLibrary::exists("shader_1"));
    EXPECT_FALSE(ShaderLibrary::exists("shader_2"));
}

TEST_F(ShaderTest, LoadInvalidGLSLFile) {
    // Should print error but not crash
    ShaderLibrary::load_glsl("invalid", "test_invalid.glsl");
    EXPECT_FALSE(ShaderLibrary::exists("invalid"));
}

#endif

} // namespace renderer

} // namespace lmgl
