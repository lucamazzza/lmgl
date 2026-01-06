#include "lmgl/assets/texture_library.hpp"
#include "lmgl/renderer/texture.hpp"

#include <iostream>

namespace lmgl {

namespace assets {

TextureLibrary& TextureLibrary::get_instance() {
    static TextureLibrary instance;
    return instance;
}

std::shared_ptr<renderer::Texture> TextureLibrary::load(const std::string& fpath) {
    auto it = m_textures.find(fpath);
    if (it != m_textures.end()) return it->second;
    auto texture = std::make_shared<renderer::Texture>(fpath);
    m_textures[fpath] = texture;
    std::cout << "Loaded texture: " << fpath << std::endl;
    return texture;
}

bool TextureLibrary::exists(const std::string& fpath) const {
    return m_textures.find(fpath) != m_textures.end();
}

std::shared_ptr<renderer::Texture> TextureLibrary::get(const std::string& fpath) const {
    auto it = m_textures.find(fpath);
    if (it != m_textures.end()) return it->second;
    return nullptr;
}

void TextureLibrary::clear() {
    m_textures.clear();
}

} // namespace assets

} // namespace lmgl
