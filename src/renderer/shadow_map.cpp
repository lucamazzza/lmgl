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
    if (!scene || !light || !shadow_map) return;
    glm::vec3 light_pos = light->get_position();
    float far_plane = light->get_range();
    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, far_plane);
    std::vector<glm::mat4> shadow_transforms;
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_depth_cubemap_shader->bind();
    m_depth_cubemap_shader->set_vec3("u_LightPos", light_pos);
    m_depth_cubemap_shader->set_float("u_FarPlane", far_plane);
    for (unsigned int i = 0; i < 6; ++i) {
        m_depth_cubemap_shader->set_mat4("u_ShadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
    }
    shadow_map->bind(0);
    glCullFace(GL_FRONT);
    render_scene_depth(scene, shadow_transforms[0]); // Pass first transform as placeholder
    glCullFace(GL_BACK);
    shadow_map->unbind();
}

void ShadowRenderer::render_scene_depth(std::shared_ptr<scene::Scene> scene, const glm::mat4 &light_space_matrix) {
    if (!scene) return;
    m_depth_shader->bind();
    m_depth_shader->set_mat4("u_LightSpaceMatrix", light_space_matrix);
    std::function<void(std::shared_ptr<scene::Node>, const glm::mat4&)> traverse;
    traverse = [&](std::shared_ptr<scene::Node> node, const glm::mat4& parent_transform) {
        if (!node) return;
        glm::mat4 transform = parent_transform * node->get_local_transform();
        auto mesh = node->get_mesh();
        if (mesh) {
            m_depth_shader->set_mat4("u_Model", transform);
            mesh->render();
        }
        for (const auto& child : node->get_children()) {
            traverse(child, transform);
        }
    };
    traverse(scene->get_root(), glm::mat4(1.0f));
}

glm::mat4 ShadowRenderer::get_light_space_matrix(std::shared_ptr<scene::Light> light, const glm::vec3& scene_center,
                                                 float scene_radius) {
    glm::vec3 light_dir = glm::normalize(light->get_direction());
    glm::vec3 light_pos = scene_center - light_dir * scene_radius;
    
    glm::mat4 light_projection = glm::ortho(-scene_radius, scene_radius,
                                           -scene_radius, scene_radius,
                                           0.1f, scene_radius * 2.0f);
    glm::mat4 light_view = glm::lookAt(light_pos, scene_center, glm::vec3(0.0f, 1.0f, 0.0f));
    
    return light_projection * light_view;
}

} // namespace renderer

} // namespace lmgl
