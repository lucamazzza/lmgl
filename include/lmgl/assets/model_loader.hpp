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

#include "lmgl/renderer/shader.hpp"
#include "lmgl/renderer/texture.hpp"
#include "lmgl/scene/mesh.hpp"
#include "lmgl/scene/node.hpp"

#include <memory>
#include <string>
#include <vector>

struct aiScene;    //!< Forward declarations for Assimp Scene structure
struct aiNode;     //!< Forward declarations for Assimp Node structure
struct aiMesh;     //!< Forward declarations for Assimp Mesh structure
struct aiMaterial; //!< Forward declarations for Assimp Material structure

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
    /*!
     * @brief Load a 3D model from a file.
     *
     * This function loads a 3D model from the specified file path using the Assimp library.
     * It processes the model's meshes, materials, and textures, and constructs a scene graph
     * representation of the model.
     *
     * @param fpath The file path to the 3D model.
     * @param shader A shared pointer to the shader to be used for rendering the model.
     * @param options Options for loading the model.
     * @return A shared pointer to the root node of the loaded model's scene graph.
     */
    static std::shared_ptr<scene::Node> load(const std::string &fpath, std::shared_ptr<renderer::Shader> shader,
                                             const ModelLoadOptions &options);

  private:
    /*!
     * @brief Process an Assimp node and its children.
     *
     * This function processes an Assimp node, converting it into a scene graph node.
     * It recursively processes all child nodes and their meshes.
     *
     * @param ai_node The Assimp node to process.
     * @param ai_scene The Assimp scene containing the node.
     * @param dir The directory of the model file, used for loading textures.
     * @param shader A shared pointer to the shader to be used for rendering the model.
     * @return A shared pointer to the processed scene graph node.
     */
    static std::shared_ptr<scene::Node> process_node(aiNode *ai_node, const aiScene *ai_scene, const std::string &dir,
                                                     std::shared_ptr<renderer::Shader> shader);

    /*!
     * @brief Process an Assimp mesh and convert it to a Mesh object.
     *
     * This function processes an Assimp mesh, extracting its vertices, indices,
     * and material information, and converts it into a Mesh object.
     *
     * @param ai_mesh The Assimp mesh to process.
     * @param ai_scene The Assimp scene containing the mesh.
     * @param dir The directory of the model file, used for loading textures.
     * @param shader A shared pointer to the shader to be used for rendering the mesh.
     * @return A shared pointer to the processed Mesh object.
     */
    static std::shared_ptr<scene::Mesh> process_mesh(aiMesh *ai_mesh, const aiScene *ai_scene, const std::string &dir,
                                                     std::shared_ptr<renderer::Shader> shader);

    /*!
     * @brief Load textures from an Assimp material.
     *
     * This function loads textures of a specified type from an Assimp material.
     * It retrieves the texture file paths and loads the textures into Texture objects.
     *
     * @param ai_material The Assimp material to load textures from.
     * @param type The type of texture to load (e.g., diffuse, specular).
     * @param dir The directory of the model file, used for loading textures.
     * @return A vector of shared pointers to the loaded Texture objects.
     */
    static std::vector<std::shared_ptr<renderer::Texture>>
    load_material_textures(aiMaterial *ai_material, unsigned int type, const std::string &dir);
    /*!
     * @brief Get the directory from a file path.
     *
     * This function extracts the directory part from a given file path.
     * It is used to determine the base directory for loading related assets such as textures.
     *
     * @param filepath The file path to extract the directory from.
     * @return The directory part of the file path.
     */
    static std::string get_directory(const std::string &filepath);

    /*!
     * @brief Load a Level of Detail (LOD) model from multiple file paths.
     *
     * This function loads a Level of Detail (LOD) model from multiple file paths,
     * each corresponding to a different level of detail. It constructs an LOD object
     * that can switch between different models based on distance.
     *
     * @param file_paths A vector of file paths for the different LOD models.
     * @param distances A vector of distances at which to switch between LOD models.
     * @param shader A shared pointer to the shader to be used for rendering the LOD models.
     * @param options Options for loading the models.
     * @return A shared pointer to the loaded LOD object.
     */
    static std::shared_ptr<scene::LOD> load_lod(const std::vector<std::string> &file_paths,
                                                const std::vector<float> &distances,
                                                std::shared_ptr<renderer::Shader> shader,
                                                const ModelLoadOptions &options = {});
};

} // namespace assets

} // namespace lmgl
