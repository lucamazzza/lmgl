/*!
 * @file text.cpp
 * @brief Implementation of the Text UI element.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/text.hpp"
#include "lmgl/ui/font.hpp"
#include "lmgl/renderer/buffer.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <glad/glad.h>

namespace lmgl {

namespace ui {

std::shared_ptr<renderer::Shader> Text::s_shader = nullptr;
std::shared_ptr<renderer::VertexArray> Text::s_vao = nullptr;
std::shared_ptr<renderer::VertexBuffer> Text::s_vbo = nullptr;
bool Text::s_initialized = false;

Text::Text(const std::string &text, const std::string &name)
    : UIElement(name), m_text(text) {}

void Text::initialize_resources() {
    if (s_initialized)
        return;

    s_shader = renderer::Shader::from_glsl_file("shaders/ui_text.glsl");

    s_vao = std::make_shared<renderer::VertexArray>();

    // Create dynamic vertex buffer (large enough for long strings)
    s_vbo = std::make_shared<renderer::VertexBuffer>(nullptr, 4096 * sizeof(float), true);
    s_vbo->set_layout({
        {renderer::ShaderDataType::Float2, "a_position"},
        {renderer::ShaderDataType::Float2, "a_tex_coord"}
    });

    s_vao->add_vertex_buffer(s_vbo);

    s_initialized = true;
}

void Text::set_text(const std::string &text) {
    m_text = text;
    update_size();
}

void Text::set_font(std::shared_ptr<Font> font) {
    m_font = font;
    update_size();
}

void Text::update_size() {
    if (!m_font || m_text.empty()) {
        m_size = glm::vec2(0.0f, 0.0f);
        return;
    }

    float width = m_font->measure_text(m_text);
    float height = m_font->get_line_height();
    m_size = glm::vec2(width, height);
}

void Text::render(float canvas_width, float canvas_height, const glm::mat4 &projection) {
    if (!m_visible || !m_font || m_text.empty())
        return;

    initialize_resources();

    glm::vec2 abs_pos = get_absolute_position(canvas_width, canvas_height);

    // Apply alignment offset
    float x_offset = 0.0f;
    if (m_alignment == TextAlign::Center) {
        x_offset = -m_size.x * 0.5f;
    } else if (m_alignment == TextAlign::Right) {
        x_offset = -m_size.x;
    }

    float x = abs_pos.x + x_offset;
    float y = abs_pos.y;

    s_shader->bind();
    s_shader->set_mat4("u_projection", projection);
    s_shader->set_vec4("u_color", m_color);
    s_shader->set_int("u_text", 0);

    m_font->get_atlas()->bind(0);

    s_vao->bind();

    // Batch render all characters
    std::vector<float> vertices;
    vertices.reserve(m_text.size() * 24); // 6 vertices * 4 floats per character

    for (char c : m_text) {
        const Glyph &glyph = m_font->get_glyph(c);

        float xpos = x + glyph.bearing.x;
        float ypos = y + (m_font->get_line_height() - glyph.bearing.y);

        float w = glyph.size.x;
        float h = glyph.size.y;

        // Add 6 vertices (2 triangles) for this character
        // Triangle 1
        vertices.push_back(xpos);
        vertices.push_back(ypos + h);
        vertices.push_back(glyph.tex_coord_min.x);
        vertices.push_back(glyph.tex_coord_max.y);

        vertices.push_back(xpos);
        vertices.push_back(ypos);
        vertices.push_back(glyph.tex_coord_min.x);
        vertices.push_back(glyph.tex_coord_min.y);

        vertices.push_back(xpos + w);
        vertices.push_back(ypos);
        vertices.push_back(glyph.tex_coord_max.x);
        vertices.push_back(glyph.tex_coord_min.y);

        // Triangle 2
        vertices.push_back(xpos);
        vertices.push_back(ypos + h);
        vertices.push_back(glyph.tex_coord_min.x);
        vertices.push_back(glyph.tex_coord_max.y);

        vertices.push_back(xpos + w);
        vertices.push_back(ypos);
        vertices.push_back(glyph.tex_coord_max.x);
        vertices.push_back(glyph.tex_coord_min.y);

        vertices.push_back(xpos + w);
        vertices.push_back(ypos + h);
        vertices.push_back(glyph.tex_coord_max.x);
        vertices.push_back(glyph.tex_coord_max.y);

        x += glyph.advance;
    }

    // Upload all vertices at once and render
    if (!vertices.empty()) {
        s_vbo->bind();
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    }

    s_vao->unbind();
    s_shader->unbind();
}

} // namespace ui

} // namespace lmgl
