#include "lmgl/renderer/renderer.hpp"
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
}

void Renderer::render(std::shared_ptr<scene::Scene> scene, std::shared_ptr<scene::Camera> camera) {
    if (!scene || !camera)
        return;
    m_draw_calls = 0;
    m_triangles_count = 0;
    m_render_queue.clear();
    glm::mat4 identity(1.0f);
    build_render_queue(scene->get_root(), camera, identity, m_render_queue);
    collect_lights(scene);
    sort_render_queue(m_render_queue);
    apply_render_mode();
    for (const auto &item : m_render_queue) {
        render_mesh(item.mesh, item.transform, camera);
    }
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

void Renderer::sort_render_queue(std::vector<RenderItem> &items) {
    std::sort(items.begin(), items.end(), [](const RenderItem &a, const RenderItem &b) {
        auto shader_a = a.mesh->get_shader().get();
        auto shader_b = b.mesh->get_shader().get();
        if (shader_a != shader_b) {
            return shader_a < shader_b;
        }
        if (a.is_transparent)
            return a.distance_to_camera > b.distance_to_camera;
        else
            return a.distance_to_camera < b.distance_to_camera;
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
    auto material = mesh->get_material();
    if (material)
        material->bind(shader);
    else {
        // Set default material
        // TODO: save it as default material
        shader->set_vec3("u_Material.albedo", glm::vec3(1.0f));
        shader->set_float("u_Material.metallic", 0.0f);
        shader->set_float("u_Material.roughness", 0.5f);
        shader->set_float("u_Material.ao", 1.0f);
        shader->set_vec3("u_Material.emissive", glm::vec3(0.0f));
        shader->set_int("u_Material.hasAlbedoMap", 0);
        shader->set_int("u_Material.hasNormalMap", 0);
        shader->set_int("u_Material.hasMetallicMap", 0);
        shader->set_int("u_Material.hasRoughnessMap", 0);
        shader->set_int("u_Material.hasAoMap", 0);
        shader->set_int("u_Material.hasEmissiveMap", 0);
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

} // namespace renderer

} // namespace lmgl
