#include "lmgl/scene/mesh.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "lmgl/renderer/buffer.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <glad/glad.h>

#include <cmath>
#include <memory>

namespace lmgl {

namespace scene {

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           std::shared_ptr<renderer::Shader> shader)
    : m_vertices(vertices), m_indices(indices), m_shader(shader), m_index_count(indices.size()) {
    setup_mesh();
}

Mesh::Mesh(const std::shared_ptr<renderer::VertexArray> vao,
           const std::shared_ptr<renderer::Shader> shader,
           unsigned int index_count)
    : m_vertex_array(vao), m_shader(shader), m_index_count(index_count) {}

void Mesh::setup_mesh() {
    auto vbo = std::make_shared<renderer::VertexBuffer>(
            m_vertices.data(),
            m_vertices.size() * sizeof(Vertex)
    );
    renderer::BufferLayout layout = {
        { renderer::ShaderDataType::Float3, "a_Position" },
        { renderer::ShaderDataType::Float3, "a_Normal" },
        { renderer::ShaderDataType::Float4, "a_Color" },
        { renderer::ShaderDataType::Float2, "a_TexCoords" },
        { renderer::ShaderDataType::Float3, "a_Tangent" },
        { renderer::ShaderDataType::Float3, "a_Bitangent" }
    };
    vbo->set_layout(layout);
    auto ibo = std::make_shared<renderer::IndexBuffer>(
            m_indices.data(),
            m_indices.size()
    );
    m_vertex_array = std::make_shared<renderer::VertexArray>();
    m_vertex_array->add_vertex_buffer(vbo);
    m_vertex_array->set_index_buffer(ibo);
}

void Mesh::bind() const {
    if (m_vertex_array) m_vertex_array->bind();
    if (m_shader) m_shader->bind();
}

void Mesh::unbind() const {
    if (m_shader) m_shader->unbind();
    if (m_vertex_array) m_vertex_array->unbind();
}

void Mesh::render() const {
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
}

std::shared_ptr<Mesh> Mesh::create_cube(std::shared_ptr<renderer::Shader> shader, unsigned int subdivisions) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    subdivisions = std::max(1u, std::min(subdivisions, 50u));
    const float step = 1.0f / subdivisions;
    
    auto generate_face = [&](
            const glm::vec3& origin,
            const glm::vec3& right,
            const glm::vec3& up,
            const glm::vec3& normal) {
        unsigned int start_idx = vertices.size();
        for (unsigned int y = 0; y <= subdivisions; ++y) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                float u = x * step;
                float v = y * step;
                glm::vec3 pos = origin + right * (u - 0.5f) + up * (v - 0.5f);
                glm::vec2 uv(u, v);
                vertices.push_back({ pos, normal, glm::vec4(1.0f), uv });
            }
        }
        for (unsigned int y = 0; y < subdivisions; ++y) {
            for (unsigned int x = 0; x < subdivisions; ++x) {
                unsigned int i0 = start_idx + y * (subdivisions + 1) + x;
                unsigned int i1 = i0 + 1;
                unsigned int i2 = i0 + (subdivisions + 1);
                unsigned int i3 = i2 + 1;
                indices.push_back(i0);
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);
            }
        }
    };
    generate_face(glm::vec3(0, 0, 0.5f),  glm::vec3(1, 0, 0),  glm::vec3(0, 1, 0),  glm::vec3(0, 0, 1));
    generate_face(glm::vec3(0, 0, -0.5f), glm::vec3(1, 0, 0),  glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
    generate_face(glm::vec3(0, 0.5f, 0),  glm::vec3(1, 0, 0),  glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    generate_face(glm::vec3(0, -0.5f, 0), glm::vec3(1, 0, 0),  glm::vec3(0, 0, 1),  glm::vec3(0, -1, 0));
    generate_face(glm::vec3(0.5f, 0, 0),  glm::vec3(0, 0, -1), glm::vec3(0, 1, 0),  glm::vec3(1, 0, 0));
    generate_face(glm::vec3(-0.5f, 0, 0), glm::vec3(0, 0, 1),  glm::vec3(0, 1, 0),  glm::vec3(-1, 0, 0));
    return std::make_shared<Mesh>(vertices, indices, shader);
}

std::shared_ptr<Mesh> Mesh::create_quad(std::shared_ptr<renderer::Shader> shader, float width, float height) {
    float halfw = width / 2.0f;
    float halfh = height / 2.0f;
    std::vector<Vertex> vertices = {
        { glm::vec3(-halfw, -halfh, 0.0f), glm::vec3(0, 0, 1), glm::vec4(1.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3( halfw, -halfh, 0.0f), glm::vec3(0, 0, 1), glm::vec4(1.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3( halfw,  halfh, 0.0f), glm::vec3(0, 0, 1), glm::vec4(1.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-halfw,  halfh, 0.0f), glm::vec3(0, 0, 1), glm::vec4(1.0f), glm::vec2(0.0f, 1.0f) }
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };
    return std::make_shared<Mesh>(vertices, indices, shader);
}

std::shared_ptr<Mesh> Mesh::create_sphere(
         std::shared_ptr<renderer::Shader> shader,
         float radius,
         unsigned int latsegs,
         unsigned int lonsegs
    ) {
         std::vector<Vertex> vertices;
         std::vector<unsigned int> indices;
         latsegs = std::max(3u, std::min(latsegs, 500u));
         lonsegs = std::max(3u, std::min(lonsegs, 500u));
         
         for (unsigned int lat = 0; lat <= latsegs; ++lat) {
             float theta = lat * glm::pi<float>() / latsegs;
             float sin_theta = std::sin(theta);
             float cos_theta = std::cos(theta);
             
             for (unsigned int lon = 0; lon <= lonsegs; ++lon) {
                 float phi = lon * 2.0f * glm::pi<float>() / lonsegs;
                 float sin_phi = std::sin(phi);
                 float cos_phi = std::cos(phi);
                 
                 glm::vec3 position(
                     radius * sin_theta * cos_phi,
                     radius * cos_theta,
                     radius * sin_theta * sin_phi
                 );
                 glm::vec3 normal = glm::normalize(position);
                 glm::vec2 uv(
                     static_cast<float>(lon) / lonsegs,
                     static_cast<float>(lat) / latsegs
                 );
                 vertices.push_back({ position, normal, glm::vec4(1.0f), uv });
             }
         }
         
         for (unsigned int lat = 0; lat < latsegs; ++lat) {
             for (unsigned int lon = 0; lon < lonsegs; ++lon) {
                 unsigned int first = (lat * (lonsegs + 1)) + lon;
                 unsigned int second = first + lonsegs + 1;
                 
                 // Triangle 1: counter-clockwise winding
                 indices.push_back(first);
                 indices.push_back(first + 1);
                 indices.push_back(second);
                 
                 // Triangle 2: counter-clockwise winding
                 indices.push_back(second);
                 indices.push_back(first + 1);
                 indices.push_back(second + 1);
             }
         }
         
         return std::make_shared<Mesh>(vertices, indices, shader);
     }

} // namespace scene

} // namespace lmgl
