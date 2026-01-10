#include "lmgl/assets/model_loader.hpp"
#include "lmgl/assets/texture_library.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>
#include <iostream>
#include <memory>

namespace lmgl {

namespace assets {

std::shared_ptr<scene::Node> ModelLoader::load(const std::string &fpath, std::shared_ptr<renderer::Shader> shader,
                                               const ModelLoadOptions &options) {
    Assimp::Importer importer;
    unsigned int flags = 0;
    if (options.triangulate) flags |= aiProcess_Triangulate;
    if (options.flip_uvs) flags |= aiProcess_FlipUVs;
    if (options.compute_tangents) flags |= aiProcess_CalcTangentSpace;
    if (options.optimize_meshes) {
        flags |= aiProcess_OptimizeMeshes;
        flags |= aiProcess_OptimizeGraph;
    }
    flags |= aiProcess_GenNormals;
    flags |= aiProcess_JoinIdenticalVertices;
    const aiScene *ai_scene = importer.ReadFile(fpath, flags);
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
        std::cerr << "ERROR: Assimp failed to load model: " << fpath << std::endl;
        std::cerr << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    std::string dir = get_directory(fpath);
    std::cout << "ModelLoader: Loading model from " << fpath << std::endl;
    std::cout << "  Meshes: " << ai_scene->mNumMeshes << std::endl;
    std::cout << "  Materials: " << ai_scene->mNumMaterials << std::endl;
    std::cout << "  Textures: " << ai_scene->mNumTextures << std::endl;
    auto root_node = process_node(ai_scene->mRootNode, ai_scene, dir, shader);
    if (options.scale != 1.0f) {
        root_node->set_scale(glm::vec3(options.scale));
    }
    std::cout << "ModelLoader: Finished loading model." << std::endl;
    return root_node;
}

std::shared_ptr<scene::Node> ModelLoader::process_node(aiNode *ai_node, const aiScene *ai_scene, const std::string &dir,
                                                       std::shared_ptr<renderer::Shader> shader) {
    auto node = std::make_shared<scene::Node>(ai_node->mName.C_Str());
    for (unsigned int i = 0; i < ai_node->mNumMeshes; ++i) {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        auto mesh = process_mesh(ai_mesh, ai_scene, dir, shader);
        if (mesh) {
            if (ai_node->mNumMeshes == 1) {
                node->set_mesh(mesh);
            } else {
                auto mesh_node =
                    std::make_shared<scene::Node>(std::string(ai_mesh->mName.C_Str()) + "_mesh_" + std::to_string(i));
                mesh_node->set_mesh(mesh);
                node->add_child(mesh_node);
            }
        }
    }
    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        auto child_node = process_node(ai_node->mChildren[i], ai_scene, dir, shader);
        if (child_node)
            node->add_child(child_node);
    }
    return node;
}

std::shared_ptr<scene::Mesh> ModelLoader::process_mesh(aiMesh *ai_mesh, const aiScene *ai_scene, const std::string &dir,
                                                       std::shared_ptr<renderer::Shader> shader) {
    std::vector<scene::Vertex> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
        scene::Vertex vertex;
        // position
        vertex.position = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
        // normal
        if (ai_mesh->HasNormals()) {
            vertex.normal = glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);
        } else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        // texture coordinates
        if (ai_mesh->mTextureCoords[0]) {
            vertex.uvs = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);
        } else {
            vertex.uvs = glm::vec2(0.0f, 0.0f);
        }
        // color
        if (ai_mesh->HasVertexColors(0)) {
            vertex.color = glm::vec4(ai_mesh->mColors[0][i].r, ai_mesh->mColors[0][i].g, ai_mesh->mColors[0][i].b,
                                     ai_mesh->mColors[0][i].a);
        } else {
            vertex.color = glm::vec4(1.0f);
        }
        if (ai_mesh->HasTangentsAndBitangents()) {
            vertex.tangent = glm::vec3(ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z);
            vertex.bitangent =
                glm::vec3(ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z);
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
        aiFace face = ai_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    // Create mesh
    auto mesh = std::make_shared<scene::Mesh>(vertices, indices, shader);

    // Load and attach material
    if (ai_mesh->mMaterialIndex >= 0) {
        aiMaterial *ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
        auto material = std::make_shared<scene::Material>(ai_material->GetName().C_Str());
        // Load PBR properties
        aiColor3D color;
        float value;
        // Albedo/Diffuse
        if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material->set_albedo(glm::vec3(color.r, color.g, color.b));
        }
        // Metallic
        if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
            material->set_metallic(value);
        }
        // Roughness
        if (ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
            material->set_roughness(value);
        }
        // Emissive
        if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            material->set_emissive(glm::vec3(color.r, color.g, color.b));
        }
        // Load texture maps
        auto &tex_lib = TextureLibrary::get_instance();
        // Albedo/Diffuse map
        auto diffuse_maps = load_material_textures(ai_material, aiTextureType_DIFFUSE, dir);
        if (!diffuse_maps.empty()) {
            material->set_albedo_map(diffuse_maps[0]);
        }
        // Normal map
        auto normal_maps = load_material_textures(ai_material, aiTextureType_NORMALS, dir);
        if (!normal_maps.empty()) {
            material->set_normal_map(normal_maps[0]);
        }
        // Metallic map
        auto metallic_maps = load_material_textures(ai_material, aiTextureType_METALNESS, dir);
        if (!metallic_maps.empty()) {
            material->set_metallic_map(metallic_maps[0]);
        }
        // Roughness map
        auto roughness_maps = load_material_textures(ai_material, aiTextureType_DIFFUSE_ROUGHNESS, dir);
        if (!roughness_maps.empty()) {
            material->set_roughness_map(roughness_maps[0]);
        }
        // AO map
        auto ao_maps = load_material_textures(ai_material, aiTextureType_AMBIENT_OCCLUSION, dir);
        if (!ao_maps.empty()) {
            material->set_ao_map(ao_maps[0]);
        }
        // Emissive map
        auto emissive_maps = load_material_textures(ai_material, aiTextureType_EMISSIVE, dir);
        if (!emissive_maps.empty()) {
            material->set_emissive_map(emissive_maps[0]);
        }
        mesh->set_material(material);
    }
    return mesh;
}

std::vector<std::shared_ptr<renderer::Texture>>
ModelLoader::load_material_textures(aiMaterial *ai_material, unsigned int type, const std::string &dir) {
    std::vector<std::shared_ptr<renderer::Texture>> textures;
    unsigned int count = ai_material->GetTextureCount((aiTextureType)type);
    for (unsigned int i = 0; i < count; ++i) {
        aiString str;
        ai_material->GetTexture((aiTextureType)type, i, &str);
        std::string filename = str.C_Str();
        std::string texture_path = dir + "/" + filename;
        auto &tex_lib = TextureLibrary::get_instance();
        if (std::filesystem::exists(texture_path)) {
            auto texture = tex_lib.load(texture_path);
            if (texture) {
                textures.push_back(texture);
            }
        } else {
            std::cerr << "WARNING: Texture file not found: " << texture_path << std::endl;
        }
    }
    return textures;
}

std::string ModelLoader::get_directory(const std::string &filepath) {
    size_t last_slash = filepath.find_last_of("/\\");
    if (last_slash == std::string::npos) {
        return ".";
    }
    return filepath.substr(0, last_slash);
}

} // namespace assets

} // namespace lmgl
