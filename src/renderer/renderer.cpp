#include "lmgl/renderer/renderer.hpp"
#include "lmgl/scene/frustum.hpp"
#include "lmgl/scene/light.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <memory>

namespace lmgl {

namespace renderer {

Renderer::Renderer()
    : m_render_mode(RenderMode::Solid), m_depth_test_enabled(true), m_culling_enabled(true), m_blending_enabled(false),
      m_draw_calls(0), m_triangles_count(0) {
    set_depth_test(true);
    set_culling(true);
    set_blending(false);
    m_default_material = std::make_shared<scene::Material>("DefaultMaterial");
    m_default_material->set_albedo(glm::vec3(1.0f));
    m_default_material->set_metallic(0.0f);
    m_default_material->set_roughness(0.5f);
    m_default_material->set_ao(1.0f);
    m_default_material->set_emissive(glm::vec3(0.0f));
    m_framebuffer = std::make_unique<Framebuffer>(1920, 1080, true);
    m_postprocess_shader = Shader::from_glsl_file("shaders/postprocess.glsl");
    m_bright_pass_shader = Shader::from_glsl_file("shaders/bright_pass.glsl");
    m_blur_shader = Shader::from_glsl_file("shaders/blur.glsl");
    m_bright_framebuffer = std::make_unique<Framebuffer>(1920, 1080, true);
    m_blur_framebuffer[0] = std::make_unique<Framebuffer>(1920, 1080, true);
    m_blur_framebuffer[1] = std::make_unique<Framebuffer>(1920, 1080, true);
    m_screen_quad = scene::Mesh::create_quad(m_postprocess_shader, 2.0f, 2.0f);
}

void Renderer::render(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> camera) {
    if (!scene || !camera)
        return;
    m_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_draw_calls = 0;
    m_triangles_count = 0;
    m_render_queue.clear();
    clear_material_cache();
    scene::Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    glm::mat4 identity(1.0f);
    build_render_queue_culled(scene->get_root(), camera, identity, m_render_queue, frustum);
    collect_lights(scene);
    sort_render_queue(m_render_queue);
    apply_render_mode();

    // Render scene to framebuffer
    m_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render skybox first (if present)
    if (scene->get_skybox()) {
        scene->get_skybox()->render(camera);
    }

    for (const auto &item : m_render_queue) {
        render_mesh(item.mesh, item.transform, camera);
    }

    // Post-process pass
    m_framebuffer->unbind();

    // Reset to solid fill mode for post-processing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glViewport(0, 0, m_window_width, m_window_height);
    glDisable(GL_DEPTH_TEST);

    // Bloom effect
    if (m_bloom_enabled) {

        // Extract bright pixels
        m_bright_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_bright_pass_shader->bind();
        m_bright_pass_shader->set_int("u_Texture", 0);
        m_bright_pass_shader->set_float("u_Threshold", m_bloom_threshold);
        m_framebuffer->get_color_attachment()->bind(0);
        if (m_screen_quad->get_vertex_array())
            m_screen_quad->get_vertex_array()->bind();
        m_screen_quad->render();
        m_bright_pass_shader->unbind();

        // Gaussian blur
        bool horizontal = true;
        int blur_passes = 10; // 5 iterations horizontal + 5 vertical
        float weights[5] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
        m_blur_shader->bind();
        m_blur_shader->set_float("u_Weights[0]", weights[0]);
        m_blur_shader->set_float("u_Weights[1]", weights[1]);
        m_blur_shader->set_float("u_Weights[2]", weights[2]);
        m_blur_shader->set_float("u_Weights[3]", weights[3]);
        m_blur_shader->set_float("u_Weights[4]", weights[4]);
        for (int i = 0; i < blur_passes; i++) {
            m_blur_framebuffer[horizontal ? 1 : 0]->bind();
            glClear(GL_COLOR_BUFFER_BIT);
            if (i == 0) {
                m_bright_framebuffer->get_color_attachment()->bind(0);
            } else {
                m_blur_framebuffer[horizontal ? 0 : 1]->get_color_attachment()->bind(0);
            }
            m_blur_shader->set_int("u_Horizontal", horizontal ? 1 : 0);
            m_blur_shader->set_int("u_Texture", 0);
            if (m_screen_quad->get_vertex_array())
                m_screen_quad->get_vertex_array()->bind();
            m_screen_quad->render();
            horizontal = !horizontal;
        }
        m_blur_shader->unbind();
    }

    // Final composite with tone mapping
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_postprocess_shader->bind();
    m_postprocess_shader->set_int("u_ScreenTexture", 0);
    m_postprocess_shader->set_int("u_BloomTexture", 1);
    m_postprocess_shader->set_int("u_ToneMapMode", m_tone_map_mode);
    m_postprocess_shader->set_float("u_Exposure", m_exposure);
    m_postprocess_shader->set_float("u_Gamma", m_gamma);
    m_postprocess_shader->set_int("u_BloomEnabled", m_bloom_enabled ? 1 : 0);
    m_framebuffer->get_color_attachment()->bind(0);
    if (m_bloom_enabled) {
        m_postprocess_shader->set_float("u_BloomIntensity", m_bloom_intensity);
        m_blur_framebuffer[0]->get_color_attachment()->bind(1);
    } else {
        m_postprocess_shader->set_float("u_BloomIntensity", 0.0f);
    }
    if (m_screen_quad->get_vertex_array())
        m_screen_quad->get_vertex_array()->bind();
    m_screen_quad->render();
    m_postprocess_shader->unbind();
    if (m_depth_test_enabled)
        glEnable(GL_DEPTH_TEST);
}

void Renderer::set_render_mode(RenderMode mode) { m_render_mode = mode; }

void Renderer::set_depth_test(bool enabled) {
    m_depth_test_enabled = enabled;
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::set_culling(bool enabled) {
    m_culling_enabled = enabled;
    if (enabled) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::set_blending(bool enabled) {
    m_blending_enabled = enabled;
    if (enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void Renderer::apply_render_mode() {
    switch (m_render_mode) {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Points:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(5.0f);
        break;
    case RenderMode::Solid:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

void Renderer::build_render_queue(std::shared_ptr<scene::Node> node, std::shared_ptr<scene::Camera> camera,
                                  const glm::mat4 &par_transform, std::vector<RenderItem> &out_items) {
    if (!node)
        return;
    glm::mat4 cur_transform = par_transform * node->get_local_transform();
    if (node->get_mesh()) {
        RenderItem item;
        item.mesh = node->get_mesh();
        item.transform = cur_transform;
        glm::vec3 mesh_pos(cur_transform[3]);
        glm::vec3 cam_pos(camera->get_position());
        item.distance_to_camera = glm::length(cam_pos - mesh_pos);
        item.is_transparent = false;
        item.layer = RenderLayer::Opaque;
        out_items.push_back(item);
    }
    for (const auto &child : node->get_children()) {
        build_render_queue(child, camera, cur_transform, out_items);
    }
}

void Renderer::build_render_queue_culled(std::shared_ptr<scene::Node> node, std::shared_ptr<scene::Camera> camera,
                                         const glm::mat4 &par_transform, std::vector<RenderItem> &out_items,
                                         const scene::Frustum &frustum) {
    if (!node)
        return;
    glm::mat4 cur_transform = par_transform * node->get_local_transform();
    auto mesh = node->get_mesh();
    if (mesh) {
        scene::BoundingSphere world_bounds = mesh->get_bounding_sphere().transform(cur_transform);
        if (frustum.contains_sphere(world_bounds)) {
            RenderItem item;
            item.mesh = node->get_mesh();
            item.transform = cur_transform;
            glm::vec3 mesh_pos(cur_transform[3]);
            glm::vec3 cam_pos(camera->get_position());
            item.distance_to_camera = glm::length(cam_pos - mesh_pos);
            item.is_transparent = false;
            item.layer = RenderLayer::Opaque;
            out_items.push_back(item);
        }
    }
    for (const auto &child : node->get_children()) {
        build_render_queue_culled(child, camera, cur_transform, out_items, frustum);
    }
}

void Renderer::sort_render_queue(std::vector<RenderItem> &items) {
    std::sort(items.begin(), items.end(), [](const RenderItem &a, const RenderItem &b) {
        if (static_cast<int>(a.layer) != static_cast<int>(b.layer)) {
            return static_cast<int>(a.layer) < static_cast<int>(b.layer);
        }
        Shader *shader_a = a.mesh->get_shader().get();
        Shader *shader_b = b.mesh->get_shader().get();
        if (shader_a != shader_b) {
            return shader_a < shader_b;
        }
        scene::Material *mat_a = a.mesh->get_material().get();
        scene::Material *mat_b = b.mesh->get_material().get();
        if (mat_a != mat_b) {
            return mat_a < mat_b;
        }
        if (a.is_transparent) {
            return a.distance_to_camera > b.distance_to_camera;
        } else {
            return a.distance_to_camera < b.distance_to_camera;
        }
    });
}

void Renderer::render_mesh(std::shared_ptr<scene::Mesh> mesh, const glm::mat4 &transform,
                           std::shared_ptr<scene::Camera> camera) {
    if (!mesh || !camera)
        return;
    auto shader = mesh->get_shader();
    if (!shader)
        return;
    if (mesh->get_vertex_array())
        mesh->get_vertex_array()->bind();
    shader->bind();
    glm::mat4 view(camera->get_view_matrix());
    glm::mat4 proj(camera->get_projection_matrix());
    glm::mat4 mvp(proj * view * transform);
    shader->set_mat4("u_Model", transform);
    shader->set_mat4("u_MVP", mvp);
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    shader->set_mat3("u_NormalMatrix", normal_matrix);
    shader->set_vec3("u_CameraPos", camera->get_position());
    bind_lights(shader);
    auto material = mesh->get_material();
    if (material)
        bind_material(material, shader);
    else {
        bind_material(m_default_material, shader);
    }
    mesh->render();
    m_draw_calls++;
    m_triangles_count += mesh->get_index_count() / 3;
}

void Renderer::collect_lights(std::shared_ptr<scene::Scene> scene) {
    m_directional_lights.clear();
    m_point_lights.clear();
    m_spot_lights.clear();
    for (const auto &light : scene->get_lights()) {
        switch (light->get_type()) {
        case scene::LightType::Directional:
            m_directional_lights.push_back(light);
            break;
        case scene::LightType::Point:
            m_point_lights.push_back(light);
            break;
        case scene::LightType::Spot:
            m_spot_lights.push_back(light);
            break;
        }
    }
    collect_node_lights(scene->get_root());
}

void Renderer::collect_node_lights(std::shared_ptr<scene::Node> node) {
    if (!node)
        return;
    if (node->has_light()) {
        auto light = node->get_light();
        if (light->get_type() == scene::LightType::Point || light->get_type() == scene::LightType::Spot) {
            light->set_position(node->get_position());
        }
        switch (light->get_type()) {
        case scene::LightType::Directional:
            m_directional_lights.push_back(light);
            break;
        case scene::LightType::Point:
            m_point_lights.push_back(light);
            break;
        case scene::LightType::Spot:
            m_spot_lights.push_back(light);
            break;
        }
    }
    for (const auto &child : node->get_children()) {
        collect_node_lights(child);
    }
}

void Renderer::bind_lights(std::shared_ptr<renderer::Shader> shader) {
    if (!shader)
        return;
    int num_dir_lights = std::min((int)m_directional_lights.size(), 4);
    shader->set_int("u_NumDirLights", num_dir_lights);
    for (int i = 0; i < num_dir_lights; ++i) {
        std::string base = "u_DirLights[" + std::to_string(i) + "]";
        shader->set_vec3(base + ".direction", m_directional_lights[i]->get_direction());
        shader->set_vec3(base + ".color", m_directional_lights[i]->get_color());
        shader->set_float(base + ".intensity", m_directional_lights[i]->get_intensity());
    }
    int num_point_lights = std::min((int)m_point_lights.size(), 16);
    shader->set_int("u_NumPointLights", num_point_lights);
    for (int i = 0; i < num_point_lights; ++i) {
        std::string base = "u_PointLights[" + std::to_string(i) + "]";
        shader->set_vec3(base + ".position", m_point_lights[i]->get_position());
        shader->set_vec3(base + ".color", m_point_lights[i]->get_color());
        shader->set_float(base + ".intensity", m_point_lights[i]->get_intensity());
        shader->set_float(base + ".range", m_point_lights[i]->get_range());
    }
}

void Renderer::bind_material(std::shared_ptr<scene::Material> material, std::shared_ptr<renderer::Shader> shader) {
    if (!material || !shader)
        return;
    if (material == m_last_bound_material)
        return;
    material->bind(shader);
    m_last_bound_material = material;
}

void Renderer::clear_material_cache() { m_last_bound_material = nullptr; }

void Renderer::resize(int width, int height) {
    m_window_width = width;
    m_window_height = height;
    if (m_framebuffer)
        m_framebuffer->resize(width, height);
    if (m_bright_framebuffer)
        m_bright_framebuffer->resize(width, height);
    if (m_blur_framebuffer[0])
        m_blur_framebuffer[0]->resize(width, height);
    if (m_blur_framebuffer[1])
        m_blur_framebuffer[1]->resize(width, height);
}

void Renderer::setup_shadows(std::shared_ptr<scene::Scene> scene, std::shared_ptr<Shader> shader) {
    if (!scene || !shader)
        return;

    if (!scene->are_shadows_enabled()) {
        shader->bind();
        shader->set_int("u_UseShadows", 0);
        return;
    }
    if (!m_shadow_map) {
        int resolution = scene->get_shadow_resolution();
        m_shadow_map = std::make_shared<ShadowMap>(resolution, resolution);
    }
    if (!m_shadow_renderer) {
        m_shadow_renderer = std::make_unique<ShadowRenderer>();
    }
    auto lights = scene->get_lights();
    std::shared_ptr<scene::Light> directional_light = nullptr;
    for (auto &light : lights) {
        if (light->get_type() == scene::LightType::Directional) {
            directional_light = light;
            break;
        }
    }
    if (!directional_light) {
        shader->bind();
        shader->set_int("u_UseShadows", 0);
        return;
    }
    m_shadow_renderer->render_directional_shadow(scene, directional_light, m_shadow_map);
    glm::mat4 light_space_matrix =
        m_shadow_renderer->get_light_space_matrix(directional_light, glm::vec3(0.0f, 2.0f, 0.0f), 20.0f);
    shader->bind();
    m_shadow_map->bind_texture(15);
    shader->set_int("u_ShadowMap", 15);
    shader->set_int("u_UseShadows", 1);
    shader->set_mat4("u_LightSpaceMatrix", light_space_matrix);
}

} // namespace renderer

} // namespace lmgl
