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
    m_screen_quad = scene::Mesh::create_quad(m_postprocess_shader, 2.0f, 2.0f);
}

void Renderer::render(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> camera) {
    if (!scene || !camera)
        return;
    render_scene_to_offscreen(scene, camera);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    composite_to_backbuffer(0, 0, m_window_width, m_window_height);
}

void Renderer::render_stereo(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> left_camera,
                             std::shared_ptr<scene::Camera> right_camera) {
    if (!scene || !left_camera || !right_camera)
        return;

    if (m_window_width < 2 || m_window_height < 1) {
        render(scene, left_camera);
        return;
    }

    const int half_width = std::max(1, m_window_width / 2);
    const int right_width = std::max(1, m_window_width - half_width);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_scene_to_offscreen(scene, left_camera);
    composite_to_backbuffer(0, 0, half_width, m_window_height);

    render_scene_to_offscreen(scene, right_camera);
    composite_to_backbuffer(half_width, 0, right_width, m_window_height);

    // Restore full-window viewport for any subsequent passes (e.g. UI overlays).
    glViewport(0, 0, m_window_width, m_window_height);
}

void Renderer::render_scene_to_offscreen(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> camera) {
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

    if (scene->get_skybox()) {
        scene->get_skybox()->render(camera);
    }

    for (const auto &item : m_render_queue) {
        render_mesh(item.mesh, item.transform, camera, scene);
    }
    m_framebuffer->unbind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glViewport(0, 0, m_window_width, m_window_height);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::composite_to_backbuffer(int viewport_x, int viewport_y, int viewport_w, int viewport_h) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
    glDisable(GL_DEPTH_TEST);
    m_postprocess_shader->bind();
    m_postprocess_shader->set_int("u_ScreenTexture", 0);
    m_postprocess_shader->set_int("u_ToneMapMode", m_tone_map_mode);
    m_postprocess_shader->set_float("u_Exposure", m_exposure);
    m_postprocess_shader->set_float("u_Gamma", m_gamma);
    m_framebuffer->get_color_attachment()->bind(0);
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
                           std::shared_ptr<scene::Camera> camera, std::shared_ptr<scene::Scene> scene) {
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
    if (m_shadow_enabled) {
        if (m_shadow_map) {
            m_shadow_map->bind_texture(15);
            shader->set_int("u_ShadowMap", 15);
            shader->set_int("u_UseDirectionalShadow", 1);
            shader->set_mat4("u_LightSpaceMatrix", m_light_space_matrix);
        } else {
            shader->set_int("u_UseDirectionalShadow", 0);
        }

        if (m_cubemap_shadow_map) {
            m_cubemap_shadow_map->bind_texture(16);
            shader->set_int("u_ShadowCubemap", 16);
            shader->set_int("u_UsePointShadow", 1);
            shader->set_vec3("u_ShadowLightPos", m_shadow_light_pos);
            shader->set_float("u_ShadowFarPlane", m_shadow_far_plane);
        } else {
            shader->set_int("u_UsePointShadow", 0);
        }
    } else {
        shader->set_int("u_UseDirectionalShadow", 0);
        shader->set_int("u_UsePointShadow", 0);
    }
    if (scene->get_skybox() && scene->get_skybox()->get_cubemap()) {
        scene->get_skybox()->get_cubemap()->bind(14);
        shader->set_int("u_EnvironmentMap", 14);
        shader->set_int("u_UseEnvironmentMap", 1);
    } else {
        shader->set_int("u_UseEnvironmentMap", 0);
    }
    auto material = mesh->get_material();
    auto active_material = material ? material : m_default_material;
    bind_material(active_material, shader);

    const bool render_double_sided = active_material && active_material->is_double_sided();
    const bool disable_culling_for_mesh = m_culling_enabled && render_double_sided;
    if (disable_culling_for_mesh) {
        glDisable(GL_CULL_FACE);
    }
    mesh->render();
    if (disable_culling_for_mesh) {
        glEnable(GL_CULL_FACE);
    }
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
    int num_spot_lights = std::min((int)m_spot_lights.size(), 8);
    if (num_spot_lights > 0) {
        shader->set_int("u_NumSpotLights", num_spot_lights);
        for (int i = 0; i < num_spot_lights; ++i) {
            std::string base = "u_SpotLights[" + std::to_string(i) + "]";
            shader->set_vec3(base + ".position", m_spot_lights[i]->get_position());
            shader->set_vec3(base + ".direction", m_spot_lights[i]->get_direction());
            shader->set_vec3(base + ".color", m_spot_lights[i]->get_color());
            shader->set_float(base + ".intensity", m_spot_lights[i]->get_intensity());
            shader->set_float(base + ".range", m_spot_lights[i]->get_range());
            shader->set_float(base + ".innerCutoff", m_spot_lights[i]->get_inner_cone());
            shader->set_float(base + ".outerCutoff", m_spot_lights[i]->get_outer_cone());
        }
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
}

void Renderer::setup_shadows(std::shared_ptr<scene::Scene> scene, std::shared_ptr<Shader> shader,
                            bool enable_point, bool enable_directional) {
    if (!scene || !shader)
        return;

    if (!scene->are_shadows_enabled()) {
        m_shadow_enabled = false;
        return;
    }
    m_shadow_enabled = true;
    if (!m_shadow_renderer) {
        m_shadow_renderer = std::make_unique<ShadowRenderer>();
    }
    auto lights = scene->get_lights();
    std::shared_ptr<scene::Light> point_light = nullptr;
    std::shared_ptr<scene::Light> directional_light = nullptr;
    for (auto &light : lights) {
        if (light->get_type() == scene::LightType::Point && !point_light) {
            point_light = light;
        }
        if (light->get_type() == scene::LightType::Directional && !directional_light) {
            directional_light = light;
        }
    }
    if (enable_point && point_light) {
        if (!m_cubemap_shadow_map) {
            int resolution = scene->get_shadow_resolution();
            m_cubemap_shadow_map = std::make_shared<CubemapShadowMap>(resolution);
        }
        m_shadow_renderer->render_point_shadow(scene, point_light, m_cubemap_shadow_map);
        m_shadow_light_pos = point_light->get_position();
        m_shadow_far_plane = point_light->get_range();
    } else {
        m_cubemap_shadow_map = nullptr;
    }
    if (enable_directional && directional_light) {
        if (!m_shadow_map) {
            int resolution = scene->get_shadow_resolution();
            m_shadow_map = std::make_shared<ShadowMap>(resolution, resolution);
        }
        m_shadow_renderer->render_directional_shadow(scene, directional_light, m_shadow_map);
        m_light_space_matrix =
            m_shadow_renderer->get_light_space_matrix(directional_light, glm::vec3(0.0f, 2.0f, 0.0f), 20.0f);
    } else {
        m_shadow_map = nullptr;
    }
}

} // namespace renderer

} // namespace lmgl
