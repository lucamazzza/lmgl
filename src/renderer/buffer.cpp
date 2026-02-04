#include "lmgl/renderer/buffer.hpp"

#include <glad/glad.h>

#include <cstddef>

namespace lmgl {

namespace renderer {

static unsigned int shader_data_type_size(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:  return 4;
    case ShaderDataType::Float2: return 4 * 2;
    case ShaderDataType::Float3: return 4 * 3;
    case ShaderDataType::Float4: return 4 * 4;
    case ShaderDataType::Mat3:   return 4 * 3 * 3;
    case ShaderDataType::Mat4:   return 4 * 4 * 4;
    case ShaderDataType::Int:    return 4;
    case ShaderDataType::Int2:   return 4 * 2;
    case ShaderDataType::Int3:   return 4 * 3;
    case ShaderDataType::Int4:   return 4 * 4;
    case ShaderDataType::Bool:   return 1;
    default:                     return 0;
    }
}

// BufferElement

BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized)
    : name(name), type(type), size(shader_data_type_size(type)), offset(0), normalized(normalized) {}

unsigned int BufferElement::get_component_count() const {
    switch (type) {
    case ShaderDataType::Float:  return 1;
    case ShaderDataType::Float2: return 2;
    case ShaderDataType::Float3: return 3;
    case ShaderDataType::Float4: return 4;
    case ShaderDataType::Mat3:   return 3 * 3;
    case ShaderDataType::Mat4:   return 4 * 4;
    case ShaderDataType::Int:    return 1;
    case ShaderDataType::Int2:   return 2;
    case ShaderDataType::Int3:   return 3;
    case ShaderDataType::Int4:   return 4;
    case ShaderDataType::Bool:   return 1;
    default:                     return 0;
    }
}

// BufferLayout

BufferLayout::BufferLayout(const std::initializer_list<BufferElement> &elements) : m_elements(elements) {
    calculate_offsets_and_stride();
}

std::vector<BufferElement>::const_iterator BufferLayout::begin() const { return m_elements.begin(); }

std::vector<BufferElement>::const_iterator BufferLayout::end() const { return m_elements.end(); }

std::vector<BufferElement>::iterator BufferLayout::begin() { return m_elements.begin(); }

std::vector<BufferElement>::iterator BufferLayout::end() { return m_elements.end(); }

void BufferLayout::calculate_offsets_and_stride() {
    size_t offset = 0;
    m_stride = 0;
    for (auto &element : m_elements) {
        element.offset = offset;
        offset += element.size;
        m_stride += element.size;
    }
}

// VertexBuffer

VertexBuffer::VertexBuffer(const void *vertices, unsigned int size, bool dynamic) {
    glGenBuffers(1, &m_renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_renderer_id); }

void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id); }

void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::set_data(const void *data, unsigned int size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void VertexBuffer::set_layout(const BufferLayout &layout) { m_layout = layout; }

const BufferLayout &VertexBuffer::get_layout() const { return m_layout; }

// IndexBuffer

IndexBuffer::IndexBuffer(const unsigned int *indices, unsigned int count) : m_count(count) {
    glGenBuffers(1, &m_renderer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_renderer_id); }

void IndexBuffer::bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id); }

void IndexBuffer::unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

} // namespace renderer

} // namespace lmgl
