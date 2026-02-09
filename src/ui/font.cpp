/*!
 * @file font.cpp
 * @brief Implementation of Font and FontManager classes.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/font.hpp"
#include "lmgl/renderer/texture.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

namespace lmgl {

namespace ui {

Font::Font(const std::string &filepath, unsigned int font_size)
    : m_font_size(font_size), m_line_height(0.0f) {
    generate_atlas(filepath);
}

void Font::generate_atlas(const std::string &filepath) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("Failed to initialize FreeType");
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath.c_str(), 0, &face)) {
        FT_Done_FreeType(ft);
        throw std::runtime_error("Failed to load font: " + filepath);
    }

    FT_Set_Pixel_Sizes(face, 0, m_font_size);

    // Calculate atlas size (we'll use 16x8 grid for 128 ASCII characters)
    const unsigned int chars_per_row = 16;
    const unsigned int num_rows = 8;

    // Find maximum glyph dimensions
    unsigned int max_width = 0;
    unsigned int max_height = 0;

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }
        max_width = std::max(max_width, face->glyph->bitmap.width);
        max_height = std::max(max_height, face->glyph->bitmap.rows);
    }

    // Add padding
    max_width += 2;
    max_height += 2;

    unsigned int atlas_width = max_width * chars_per_row;
    unsigned int atlas_height = max_height * num_rows;

    // Create atlas buffer
    std::vector<unsigned char> atlas_data(atlas_width * atlas_height, 0);

    // Render glyphs to atlas
    unsigned int x_offset = 0;
    unsigned int y_offset = 0;

    m_line_height = static_cast<float>(face->size->metrics.height >> 6);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Warning: Failed to load glyph '" << c << "'" << std::endl;
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;

        // Copy glyph bitmap to atlas
        for (unsigned int row = 0; row < glyph->bitmap.rows; row++) {
            for (unsigned int col = 0; col < glyph->bitmap.width; col++) {
                unsigned int atlas_x = x_offset + col + 1;
                unsigned int atlas_y = y_offset + row + 1;
                atlas_data[atlas_y * atlas_width + atlas_x] =
                    glyph->bitmap.buffer[row * glyph->bitmap.width + col];
            }
        }

        // Store glyph metadata
        Glyph glyph_data;
        glyph_data.tex_coord_min = glm::vec2(
            static_cast<float>(x_offset + 1) / atlas_width,
            static_cast<float>(y_offset + 1) / atlas_height
        );
        glyph_data.tex_coord_max = glm::vec2(
            static_cast<float>(x_offset + 1 + glyph->bitmap.width) / atlas_width,
            static_cast<float>(y_offset + 1 + glyph->bitmap.rows) / atlas_height
        );
        glyph_data.size = glm::vec2(glyph->bitmap.width, glyph->bitmap.rows);
        glyph_data.bearing = glm::vec2(glyph->bitmap_left, glyph->bitmap_top);
        glyph_data.advance = static_cast<float>(glyph->advance.x >> 6);

        m_glyphs[c] = glyph_data;

        // Move to next position in atlas
        x_offset += max_width;
        if (x_offset + max_width > atlas_width) {
            x_offset = 0;
            y_offset += max_height;
        }
    }

    // Create OpenGL texture
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0,
                 GL_RED, GL_UNSIGNED_BYTE, atlas_data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_atlas = std::make_shared<renderer::Texture>(texture_id, atlas_width, atlas_height);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

const Glyph &Font::get_glyph(char c) const {
    auto it = m_glyphs.find(c);
    if (it != m_glyphs.end()) {
        return it->second;
    }
    // Return space glyph as fallback
    static Glyph fallback;
    return fallback;
}

float Font::measure_text(const std::string &text) const {
    float width = 0.0f;
    for (char c : text) {
        const Glyph &glyph = get_glyph(c);
        width += glyph.advance;
    }
    return width;
}

// FontManager implementation
std::unordered_map<std::string, std::shared_ptr<Font>> FontManager::s_fonts;

FontManager &FontManager::get() {
    static FontManager instance;
    return instance;
}

std::shared_ptr<Font> FontManager::load(const std::string &name, const std::string &filepath, unsigned int font_size) {
    std::string key = name + "_" + std::to_string(font_size);
    
    if (exists(key)) {
        return s_fonts[key];
    }

    try {
        auto font = std::make_shared<Font>(filepath, font_size);
        s_fonts[key] = font;
        return font;
    } catch (const std::exception &e) {
        std::cerr << "Failed to load font '" << name << "': " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Font> FontManager::get_font(const std::string &name) {
    auto it = s_fonts.find(name);
    if (it != s_fonts.end()) {
        return it->second;
    }
    return nullptr;
}

bool FontManager::exists(const std::string &name) const {
    return s_fonts.find(name) != s_fonts.end();
}

void FontManager::clear() {
    s_fonts.clear();
}

} // namespace ui

} // namespace lmgl
