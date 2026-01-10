#include "lmgl/renderer/texture.hpp"

#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lmgl {

namespace renderer {

Texture::Texture(int width, int height)
    : m_renderer_id(0), m_width(width), m_height(height), m_internal_format(GL_RGBA8), m_data_format(GL_RGBA) {
    glGenTextures(1, &m_renderer_id);
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_data_format, GL_UNSIGNED_BYTE, nullptr);
    init_texture_params();
}

Texture::Texture(const std::string &path) : m_renderer_id(0), m_file_path(path), m_width(0), m_height(0) {
    stbi_set_flip_vertically_on_load(1);
    using StbiPtr = std::unique_ptr<unsigned char, void (*)(void *)>;
    int channels;

    unsigned char *raw_data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 0);
    StbiPtr data(raw_data, stbi_image_free);
    if (data) {
        if (channels == 4) {
            m_internal_format = GL_RGBA8;
            m_data_format = GL_RGBA;
        } else if (channels == 3) {
            m_internal_format = GL_RGB8;
            m_data_format = GL_RGB;
        }
        glGenTextures(1, &m_renderer_id);
        glBindTexture(GL_TEXTURE_2D, m_renderer_id);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_data_format, GL_UNSIGNED_BYTE,
                     data.get());
        init_texture_params();
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "Cause: " << stbi_failure_reason() << std::endl;
    }
}

Texture::~Texture() { glDeleteTextures(1, &m_renderer_id); }

void Texture::init_texture_params() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_data_format, GL_UNSIGNED_BYTE, nullptr);
}

void Texture::set_data(void *data, unsigned int size) {
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE, data);
}

} // namespace renderer

} // namespace lmgl
