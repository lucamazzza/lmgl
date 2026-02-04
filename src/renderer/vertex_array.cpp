#include "lmgl/renderer/vertex_array.hpp"

#include <glad/glad.h>

namespace lmgl {

namespace renderer {

static GLenum shader_data_type_to_opengl_type(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:
    case ShaderDataType::Float2:
    case ShaderDataType::Float3:
    case ShaderDataType::Float4:
        return GL_FLOAT;
    case ShaderDataType::Mat3:
    case ShaderDataType::Mat4:
        return GL_FLOAT;
    case ShaderDataType::Int:
    case ShaderDataType::Int2:
    case ShaderDataType::Int3:
    case ShaderDataType::Int4:
    case ShaderDataType::Bool:
        return GL_BOOL;
    default:
        return 0;
    }
}

VertexArray::VertexArray() { glGenVertexArrays(1, &m_renderer_id); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_renderer_id); }

void VertexArray::bind() const { glBindVertexArray(m_renderer_id); }

void VertexArray::unbind() const { glBindVertexArray(0); }

void VertexArray::add_vertex_buffer(const std::shared_ptr<VertexBuffer> &vertex_buffer) {
    glBindVertexArray(m_renderer_id);
    vertex_buffer->bind();
    const auto &layout = vertex_buffer->get_layout();
    unsigned int index = 0;
    for (const auto &element : layout) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, element.get_component_count(), shader_data_type_to_opengl_type(element.type),
                              element.normalized ? GL_TRUE : GL_FALSE, layout.get_stride(),
                              (const void *)element.offset);
        index++;
    }
    m_vertex_buffers.push_back(vertex_buffer);
    glBindVertexArray(0);
}

void VertexArray::set_index_buffer(const std::shared_ptr<IndexBuffer> &index_buffer) {
    glBindVertexArray(m_renderer_id);
    index_buffer->bind();
    m_index_buffer = index_buffer;
    glBindVertexArray(0);
}

const std::vector<std::shared_ptr<VertexBuffer>> &VertexArray::get_vertex_buffers() const { return m_vertex_buffers; }

const std::shared_ptr<IndexBuffer> &VertexArray::get_index_buffer() const { return m_index_buffer; }

} // namespace renderer

} // namespace lmgl
