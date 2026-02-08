/*!
 * @file image.cpp
 * @brief Implementation of the Image UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/image.hpp"
#include "lmgl/renderer/buffer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace lmgl {

namespace ui {

std::shared_ptr<renderer::Shader> Image::s_shader = nullptr;
std::shared_ptr<renderer::VertexArray> Image::s_vao = nullptr;
bool Image::s_initialized = false;

Image::Image(const std::string &name) : UIElement(name) {}

void Image::initialize_resources() {
    if (s_initialized)
        return;

    s_shader = renderer::Shader::from_glsl_file("shaders/ui_image.glsl");

    float quad_vertices[] = {
        // Position   // TexCoord
        0.0f, 0.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 1.0f,   1.0f, 0.0f,
        0.0f, 1.0f,   0.0f, 0.0f
    };

    unsigned int quad_indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    s_vao = std::make_shared<renderer::VertexArray>();

    auto vbo = std::make_shared<renderer::VertexBuffer>(quad_vertices, sizeof(quad_vertices));
    vbo->set_layout({
        {renderer::ShaderDataType::Float2, "a_position"},
        {renderer::ShaderDataType::Float2, "a_tex_coord"}
    });

    auto ibo = std::make_shared<renderer::IndexBuffer>(quad_indices, 6);

    s_vao->add_vertex_buffer(vbo);
    s_vao->set_index_buffer(ibo);

    s_initialized = true;
}

void Image::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible || !m_texture)
        return;

    initialize_resources();

    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(abs_pos.x, abs_pos.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(m_size.x, m_size.y, 1.0f));

    s_shader->bind();
    s_shader->set_mat4("u_projection", projection);
    s_shader->set_mat4("u_transform", transform);
    s_shader->set_vec4("u_tint", m_tint);
    s_shader->set_int("u_texture", 0);

    m_texture->bind(0);

    s_vao->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    s_vao->unbind();

    s_shader->unbind();
}

} // namespace ui

} // namespace lmgl
