/*!
 * @file font.hpp
 * @brief Font loading and glyph atlas generation for text rendering.
 *
 * This header file contains the Font class which handles loading TrueType fonts
 * using FreeType, generating texture atlases, and providing glyph metrics.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include "lmgl/renderer/texture.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace lmgl {

namespace ui {

/*!
 * @brief Glyph metadata for a single character.
 *
 * Contains all information needed to render a character from the texture atlas.
 */
struct Glyph {
    //! @brief UV coordinates in texture atlas (min)
    glm::vec2 tex_coord_min;

    //! @brief UV coordinates in texture atlas (max)
    glm::vec2 tex_coord_max;

    //! @brief Size of glyph in pixels
    glm::vec2 size;

    //! @brief Offset from baseline to top-left of glyph
    glm::vec2 bearing;

    //! @brief Horizontal offset to advance to next glyph
    float advance;
};

/*!
 * @brief Font class for loading and rendering text.
 *
 * The Font class uses FreeType to load TrueType fonts, generates a texture
 * atlas containing all ASCII glyphs, and provides metrics for text rendering.
 */
class Font {
  public:
    /*!
     * @brief Load a font from file.
     *
     * @param filepath Path to the TTF/OTF font file.
     * @param font_size Size of the font in pixels.
     */
    Font(const std::string &filepath, unsigned int font_size = 48);

    /*!
     * @brief Destructor.
     */
    ~Font() = default;

    /*!
     * @brief Get glyph metadata for a character.
     *
     * @param c Character to get glyph for.
     * @return Const reference to the glyph data.
     */
    const Glyph &get_glyph(char c) const;

    /*!
     * @brief Get the texture atlas containing all glyphs.
     *
     * @return Shared pointer to the texture atlas.
     */
    inline std::shared_ptr<renderer::Texture> get_atlas() const { return m_atlas; }

    /*!
     * @brief Get the font size in pixels.
     *
     * @return Font size.
     */
    inline unsigned int get_font_size() const { return m_font_size; }

    /*!
     * @brief Get the line height for this font.
     *
     * @return Line height in pixels.
     */
    inline float get_line_height() const { return m_line_height; }

    /*!
     * @brief Calculate the width of a text string in pixels.
     *
     * @param text Text string to measure.
     * @return Width in pixels.
     */
    float measure_text(const std::string &text) const;

  private:
    //! @brief Font size in pixels
    unsigned int m_font_size;

    //! @brief Line height in pixels
    float m_line_height;

    //! @brief Texture atlas containing all glyphs
    std::shared_ptr<renderer::Texture> m_atlas;

    //! @brief Glyph metadata for each character
    std::unordered_map<char, Glyph> m_glyphs;

    /*!
     * @brief Generate the texture atlas and glyph metadata.
     *
     * @param filepath Path to the font file.
     */
    void generate_atlas(const std::string &filepath);
};

/*!
 * @brief Font manager for caching loaded fonts.
 *
 * Singleton class that manages font loading and caching.
 */
class FontManager {
  public:
    /*!
     * @brief Get the singleton instance.
     *
     * @return Reference to the FontManager instance.
     */
    static FontManager &get();

    /*!
     * @brief Load a font or get it from cache.
     *
     * @param name Unique name for the font.
     * @param filepath Path to the font file.
     * @param font_size Font size in pixels.
     * @return Shared pointer to the loaded font.
     */
    std::shared_ptr<Font> load(const std::string &name, const std::string &filepath, unsigned int font_size = 48);

    /*!
     * @brief Get a previously loaded font.
     *
     * @param name Name of the font.
     * @return Shared pointer to the font, or nullptr if not found.
     */
    std::shared_ptr<Font> get_font(const std::string &name);

    /*!
     * @brief Check if a font exists in the cache.
     *
     * @param name Name of the font.
     * @return True if font exists, false otherwise.
     */
    bool exists(const std::string &name) const;

    /*!
     * @brief Clear all cached fonts.
     */
    void clear();

  private:
    FontManager() = default;
    ~FontManager() = default;

    //! @brief Cached fonts
    static std::unordered_map<std::string, std::shared_ptr<Font>> s_fonts;
};

} // namespace ui

} // namespace lmgl
