#include "lmgl/renderer/framebuffer.hpp"

#include <iostream>

namespace lmgl {

namespace renderer {

Framebuffer::Framebuffer(int width, int height) 
    : m_width(width), m_height(height) {
    invalidate();
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_renderer_id);
    glDeleteRenderbuffers(1, &m_depth_attachment);
}

void Framebuffer::invalidate() {
    if (m_renderer_id) {
        glDeleteFramebuffers(1, &m_renderer_id);
        glDeleteRenderbuffers(1, &m_depth_attachment);
    }
    glGenFramebuffers(1, &m_renderer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);
    m_texture_attachment = std::make_shared<Texture>(m_width, m_height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_attachment->get_id(), 0);
    glGenRenderbuffers(1, &m_depth_attachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_attachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_attachment);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer is incomplete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);
    glViewport(0, 0, m_width, m_height);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height) {
    if (width == 0 || height == 0 || (width == m_width && height == m_height)) return;
    m_width = width;
    m_height = height;
    invalidate();
}

} // namespace renderer

} // namespace lmgl
