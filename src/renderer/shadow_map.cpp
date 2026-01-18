#include "lmgl/renderer/shadow_map.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"

#include <functional>
#include <iostream>
#include <memory>

namespace lmgl {

namespace renderer {

// ShadowMap

ShadowMap::ShadowMap(unsigned int width, unsigned int height) : m_width(width), m_height(height) {
    glGenFramebuffers(1, &m_fbo);
    glGenTextures(1, &m_depth_map);
    glBindTexture(GL_TEXTURE_2D, m_depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Shadow map framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap() {
    if (m_depth_map)
        glDeleteTextures(1, &m_depth_map);
    if (m_fbo)
        glDeleteFramebuffers(1, &m_fbo);
}

void ShadowMap::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void ShadowMap::bind_texture(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_depth_map);
}

void ShadowMap::resize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
    glBindTexture(GL_TEXTURE_2D, m_depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

// CubemapShadowMap

CubemapShadowMap::CubemapShadowMap(unsigned int resolution) : m_resolution(resolution) {
    glGenFramebuffers(1, &m_fbo);
    glGenTextures(1, &m_depth_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Cubemap shadow map framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CubemapShadowMap::~CubemapShadowMap() {
    if (m_depth_cubemap)
        glDeleteTextures(1, &m_depth_cubemap);
    if (m_fbo)
        glDeleteFramebuffers(1, &m_fbo);
}

void CubemapShadowMap::bind(unsigned int face) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_depth_cubemap,
                           0);
    glViewport(0, 0, m_resolution, m_resolution);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void CubemapShadowMap::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void CubemapShadowMap::bind_texture(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
}

// ShadowRenderer

ShadowRenderer::ShadowRenderer() {
    m_depth_shader = Shader::from_glsl_file("shaders/dir_light_depth.glsl");
    // NOTE: for Geom Shader I need to use Shader::from_source with Geometry support
    // for now use basic depth shader
    const char *depth_cube_geom = R"(
        #version 410 core
        layout (triangles) in;
        layout (triangle_strip, max_vertices=18) out;

        uniform mat4 u_ShadowMatrices[6];

        out vec4 v_FragPos;

        void main() {
            for(int face = 0; face < 6; ++face) {
                gl_Layer = face;
                for(int i = 0; i < 3; ++i) {
                    v_FragPos = gl_in[i].gl_Position;
                    gl_Position = u_ShadowMatrices[face] * v_FragPos;
                    EmitVertex();
                }
                EndPrimitive();
            }
        }
    )";
    m_depth_cubemap_shader = Shader::from_glsl_file("shaders/pt_light_depth.glsl");
}

void ShadowRenderer::render_directional_shadow(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Light> light,
                                               std::shared_ptr<ShadowMap> shadow_map) {
    if (!scene || !light || !shadow_map)
        return;
    glm::mat4 light_space_matrix = get_light_space_matrix(light, glm::vec3(0.0f), 10.0f);
    shadow_map->bind();
    glCullFace(GL_FRONT);
    render_scene_depth(scene, light_space_matrix);
    shadow_map->unbind();
}

void ShadowRenderer::render_point_shadow(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Light> light,
                                         std::shared_ptr<CubemapShadowMap> shadow_map) {
    // TODO: Implement point light shadow rendering
    // Requires rendering to all 6 cubemap faces
}

void ShadowRenderer::render_scene_depth(std::shared_ptr<scene::Scene> scene, const glm::mat4 &light_space_matrix) {
    // TODO: Traverse scene graph and render all meshes with depth shader
    // This is a simplified version, needs to impl full scene traversal
    m_depth_shader->bind();
    m_depth_shader->set_mat4("u_LightSpaceMatrix", light_space_matrix);
}

} // namespace renderer

} // namespace lmgl
