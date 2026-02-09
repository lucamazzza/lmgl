/*!
 * @file panel.cpp
 * @brief Implementation of the Panel UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/panel.hpp"
#include "lmgl/renderer/buffer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace lmgl {

namespace ui {

std::shared_ptr<renderer::Shader> Panel::s_shader = nullptr;
std::shared_ptr<renderer::VertexArray> Panel::s_vao = nullptr;
bool Panel::s_initialized = false;

Panel::Panel(const std::string &name) : UIElement(name) {}

void Panel::initialize_resources() {
    if (s_initialized)
        return;

    s_shader = renderer::Shader::from_glsl_file("shaders/ui_panel.glsl");
    
    if (!s_shader) {
        std::cerr << "ERROR: Failed to load Panel shader!" << std::endl;
        return;
    }

    float quad_vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    unsigned int quad_indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    s_vao = std::make_shared<renderer::VertexArray>();

    auto vbo = std::make_shared<renderer::VertexBuffer>(quad_vertices, sizeof(quad_vertices));
    vbo->set_layout({{renderer::ShaderDataType::Float2, "a_position"}});

    auto ibo = std::make_shared<renderer::IndexBuffer>(quad_indices, 6);

    s_vao->add_vertex_buffer(vbo);
    s_vao->set_index_buffer(ibo);

    s_initialized = true;
}

void Panel::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible)
        return;

    initialize_resources();

    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(abs_pos.x, abs_pos.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(m_size.x, m_size.y, 1.0f));

    s_shader->bind();
    s_shader->set_mat4("u_projection", projection);
    s_shader->set_mat4("u_transform", transform);
    s_shader->set_vec4("u_color", m_color);

    s_vao->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    s_vao->unbind();

    s_shader->unbind();
}

} // namespace ui

} // namespace lmgl
