#include "lmgl/scene/material.hpp"

namespace lmgl {

namespace scene {

Material::Material(const std::string &name) : m_name(name) {}

void Material::set_albedo_map(const std::shared_ptr<renderer::Texture> &texture) { m_albedo_map = texture; }

void Material::set_normal_map(const std::shared_ptr<renderer::Texture> &texture) { m_normal_map = texture; }

void Material::set_metallic_map(const std::shared_ptr<renderer::Texture> &texture) { m_metallic_map = texture; }

void Material::set_roughness_map(const std::shared_ptr<renderer::Texture> &texture) { m_roughness_map = texture; }

void Material::set_ao_map(const std::shared_ptr<renderer::Texture> &texture) { m_ao_map = texture; }

void Material::set_emissive_map(const std::shared_ptr<renderer::Texture> &texture) { m_emissive_map = texture; }

void Material::bind(const std::shared_ptr<renderer::Shader> &shader) const {
    if (!shader)
        return;
    shader->set_vec3("u_Material.albedo", m_albedo);
    shader->set_float("u_Material.metallic", m_metallic);
    shader->set_float("u_Material.roughness", m_roughness);
    shader->set_float("u_Material.ao", m_ao);
    shader->set_vec3("u_Material.emissive", m_emissive);
    int tex_slot = 0;
    if (m_albedo_map) {
        m_albedo_map->bind(tex_slot);
        shader->set_int("u_Material.albedoMap", tex_slot);
        shader->set_int("u_Material.hasAlbedoMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasAlbedoMap", 0);
    }
    if (m_normal_map) {
        m_normal_map->bind(tex_slot);
        shader->set_int("u_Material.normalMap", tex_slot);
        shader->set_int("u_Material.hasNormalMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasNormalMap", 0);
    }
    if (m_metallic_map) {
        m_metallic_map->bind(tex_slot);
        shader->set_int("u_Material.metallicMap", tex_slot);
        shader->set_int("u_Material.hasMetallicMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasMetallicMap", 0);
    }
    if (m_roughness_map) {
        m_roughness_map->bind(tex_slot);
        shader->set_int("u_Material.roughnessMap", tex_slot);
        shader->set_int("u_Material.hasRoughnessMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasRoughnessMap", 0);
    }
    if (m_ao_map) {
        m_ao_map->bind(tex_slot);
        shader->set_int("u_Material.aoMap", tex_slot);
        shader->set_int("u_Material.hasAoMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasAoMap", 0);
    }
    if (m_emissive_map) {
        m_emissive_map->bind(tex_slot);
        shader->set_int("u_Material.emissiveMap", tex_slot);
        shader->set_int("u_Material.hasEmissiveMap", 1);
        tex_slot++;
    } else {
        shader->set_int("u_Material.hasEmissiveMap", 0);
    }
}

} // namespace scene

} // namespace lmgl
