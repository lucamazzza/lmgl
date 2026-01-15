#include "lmgl/scene/skybox.hpp"
#include "lmgl/renderer/buffer.hpp"
#include "lmgl/renderer/vertex_array.hpp"

#include <iostream>
#include <memory>
#include <stb_image.h>

namespace lmgl {

namespace scene {

// Cubemap

std::shared_ptr<Cubemap> Cubemap::from_faces(const std::vector<std::string> &faces) {
    if (faces.size() != 6) {
        std::cerr << "Cubemap requires 6 faces." << std::endl;
        return nullptr;
    }
    auto cubemap = std::shared_ptr<Cubemap>();
    if (!cubemap->load_faces(faces)) {
        return nullptr;
    }
    return cubemap;
}

std::shared_ptr<Cubemap> Cubemap::from_equirectangular(const std::string &path) {
    auto cubemap = std::shared_ptr<Cubemap>();
    if (!cubemap->load_equirectangular(path)) {
        return nullptr;
    }
    return cubemap;
}

Cubemap::~Cubemap() {
    if (m_renderer_id) {
        glDeleteTextures(1, &m_renderer_id);
    }
}

bool Cubemap::load_faces(const std::vector<std::string> &faces) {
    glGenTextures(1, &m_renderer_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id);
    stbi_set_flip_vertically_on_load(0);
    int width, height, channels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        } else {
            std::cerr << "ERROR: Failed to load cubemap face: " << faces[i] << std::endl;
            std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            return false;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    std::cout << "Loaded cubemap from 6 faces" << std::endl;
    return true;
}

bool Cubemap::load_equirectangular(const std::string &path) {
    // TODO: Implement. This requires rendering the equirect onto cubemap faces using a fb and proj matr.
    std::cerr << "ERROR: Equirectangular loading not implemented yet" << std::endl;
    return false;
}

void Cubemap::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id);
}

// Skybox

Skybox::Skybox(std::shared_ptr<Cubemap> cubemap, const std::string &shader_path) : m_cubemap(cubemap) {
    m_shader = renderer::Shader::from_glsl_file(shader_path);
    if (!m_shader) {
        std::cerr << "ERROR: Failed to load skybox shader: " << shader_path << std::endl;
        return;
    }
    init_cube_geometry();
}

Skybox::~Skybox() {
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
}

void Skybox::init_cube_geometry() {
    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    m_vao = std::make_shared<renderer::VertexArray>();
    m_vao->bind();
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    m_vao->unbind();
}

void Skybox::render(std::shared_ptr<Camera> camera) {
    if (!m_cubemap || !m_shader || !m_vao) return;
    GLint old_depth_func;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_func);
    glDepthFunc(GL_LEQUAL);
    m_shader->bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera->get_view_matrix()));
    m_shader->set_mat4("u_View", view);
    m_shader->set_mat4("u_Projection", camera->get_projection_matrix());
    m_shader->set_float("u_Exposure", m_exposure);
    m_shader->set_int("u_Skybox", 0);
    m_cubemap->bind(0);
    m_vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao->bind();
    glDepthFunc(old_depth_func);
}

} // namespace scene

} // namespace lmgl
