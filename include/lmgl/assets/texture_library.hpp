/*!
 * @file texture_library.hpp
 * @brief Texture caching and management system
 *
 * This file defines the TextureLibrary class, which provides functionality
 * to load, cache, and manage textures efficiently in a graphics application.
 * It ensures that textures are loaded only once and reused across the application,
 * reducing memory usage and load times.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All right reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/texture.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace lmgl {

namespace assets {

/*!
 * @brief Manages loading and caching of textures
 *
 * The TextureLibrary class provides methods to load textures from files,
 * check for their existence in the cache, retrieve cached textures, and clear
 * the cache. It uses a singleton pattern to ensure a single instance throughout
 * the application.
 */
class TextureLibrary {
  public:
    /*!
     * @brief Get the singleton instance of the TextureLibrary
     * @return Reference to the TextureLibrary instance
     */
    static TextureLibrary &get_instance();

    /*!
     * @brief Load a texture from a file path
     *
     * If the texture has already been loaded, it returns the cached version.
     *
     * @param path The file path of the texture
     * @param srgb Whether to load the texture in sRGB color space
     * @return Shared pointer to the loaded texture
     */
    std::shared_ptr<renderer::Texture> load(const std::string &fpath);

    /*!
     * @brief Check if a texture exists in the cache
     *
     * Checks if the texture corresponding to the given file path
     * is already loaded and cached.
     *
     * @param path The file path of the texture
     * @return True if the texture is cached, false otherwise
     */
    bool exists(const std::string &fpath) const;

    /*!
     * @brief Retrieve a cached texture
     *
     * Retrieves the texture corresponding to the given file path
     * from the cache. Returns nullptr if the texture is not found.
     *
     * @param path The file path of the texture
     * @return Shared pointer to the cached texture, or nullptr if not found
     */
    std::shared_ptr<renderer::Texture> get(const std::string &fpath) const;

    /*!
     * @brief Clear the texture cache
     *
     * Removes all cached textures from the library.
     */
    void clear();

    /*!
     * @brief Get the number of cached textures
     *
     * Returns the total number of textures currently stored in the cache.
     *
     * @return The size of the texture cache
     */
    inline size_t size() const { return m_textures.size(); }

  private:
    //! @brief Private constructor for singleton pattern
    TextureLibrary() = default;

    //! @brief Destructor
    ~TextureLibrary() = default;

    //! @brief Delete copy constructor and assignment operator
    TextureLibrary(const TextureLibrary &) = delete;

    //! @brief Delete assignment operator
    TextureLibrary &operator=(const TextureLibrary &) = delete;

    //! @brief Map storing cached textures
    std::unordered_map<std::string, std::shared_ptr<renderer::Texture>> m_textures;
};

} // namespace assets

} // namespace lmgl
