/*!
 * @file model_loader.hpp
 * @brief Model loading utilities using Assimp library.
 *
 * This header provides functions to load 3D models from various file formats
 * using the Assimp library. It includes functions to load models and process their meshes and materials.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/scene/node.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"

#include <memory>
#include <string>
#include <vector>

struct aiScene;     //!< Forward declarations for Assimp Scene structure
struct aiNode;      //!< Forward declarations for Assimp Node structure
struct aiMesh;      //!< Forward declarations for Assimp Mesh structure
struct aiMaterial;  //!< Forward declarations for Assimp Material structure


namespace lmgl {

/**!
 * @brief Namespace for asset loading utilities.
 *
 * This namespace contains functions and classes related to loading various assets,
 * including 3D models, textures, and shaders.
 */
namespace assets {

/*!
 * @brief Options for loading 3D models.
 *
 * This structure defines various options that can be set when loading a 3D model
 * using the Assimp library. These options control how the model is processed and
 * optimized during loading.
 */
struct ModelLoadOptions {
    bool flip_uvs = true;         //!< Whether to flip UV coordinates
    bool compute_tangents = true; //!< Whether to compute tangents for normal mapping
    bool optimize_meshes = true;  //!< Whether to optimize meshes for better performance
    bool triangulate = true;      //!< Whether to triangulate meshes (convert polygons to triangles)
    float scale = 1.0f;           //!< Scale factor to apply to the model
};

/*!
 * @brief Class for loading 3D models using Assimp.
 *
 * This class provides functions to load 3D models from various file formats
 * using the Assimp library. It handles the processing of meshes, materials,
 * and textures, and constructs a scene graph representation of the model.
 */
class ModelLoader {
public:
    static std::shared_ptr<scene::Node> load(aiNode* ai_node,
                                             const aiScene* ai_scene,
                                             const std::string& dir,
                                             std::shared_ptr<renderer::Shader> shader);
    static std::shared_ptr<scene::Mesh> process_mesh(aiMesh* ai_mesh,
                                                     const aiScene* ai_scene,
                                                     const std::string& dir,
                                                     std::shared_ptr<renderer::Shader> shader);
    static std::vector<std::shared_ptr<renderer::Texture>> load_material_textures(aiMaterial* ai_material,
                                                                                  unsigned int type,
                                                                                  const std::string& dir);
    static std::string get_directory(const std::string& filepath);

};

} // namespace assets

} // namespace lmgl


