/*!
 * @file ui_loader.cpp
 * @brief Implementation of JSON-based UI loader.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#include "lmgl/ui/ui_loader.hpp"
#include "lmgl/ui/button.hpp"
#include "lmgl/ui/font.hpp"
#include "lmgl/ui/image.hpp"
#include "lmgl/ui/panel.hpp"
#include "lmgl/ui/slider.hpp"
#include "lmgl/ui/text.hpp"
#include "lmgl/ui/toggle.hpp"

#include <json.hpp>

#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace lmgl {

namespace ui {

// Helper functions
namespace {

Anchor parse_anchor(const std::string &anchor_str) {
    if (anchor_str == "TopLeft")
        return Anchor::TopLeft;
    if (anchor_str == "TopCenter")
        return Anchor::TopCenter;
    if (anchor_str == "TopRight")
        return Anchor::TopRight;
    if (anchor_str == "CenterLeft")
        return Anchor::CenterLeft;
    if (anchor_str == "Center")
        return Anchor::Center;
    if (anchor_str == "CenterRight")
        return Anchor::CenterRight;
    if (anchor_str == "BottomLeft")
        return Anchor::BottomLeft;
    if (anchor_str == "BottomCenter")
        return Anchor::BottomCenter;
    if (anchor_str == "BottomRight")
        return Anchor::BottomRight;
    return Anchor::TopLeft; // Default
}

TextAlign parse_text_align(const std::string &align_str) {
    if (align_str == "Left")
        return TextAlign::Left;
    if (align_str == "Center")
        return TextAlign::Center;
    if (align_str == "Right")
        return TextAlign::Right;
    return TextAlign::Left; // Default
}

glm::vec2 parse_vec2(const json &j) {
    if (j.is_array() && j.size() >= 2) {
        return glm::vec2(j[0].get<float>(), j[1].get<float>());
    }
    return glm::vec2(0.0f);
}

glm::vec4 parse_vec4(const json &j) {
    if (j.is_array() && j.size() >= 4) {
        return glm::vec4(j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>());
    }
    return glm::vec4(1.0f);
}

} // namespace

// UICallbackRegistry implementation

void UICallbackRegistry::register_callback(const std::string &name, std::function<void()> callback) {
    m_callbacks[name] = callback;
}

std::function<void()> UICallbackRegistry::get_callback(const std::string &name) const {
    auto it = m_callbacks.find(name);
    if (it != m_callbacks.end()) {
        return it->second;
    }
    return nullptr;
}

bool UICallbackRegistry::has_callback(const std::string &name) const {
    return m_callbacks.find(name) != m_callbacks.end();
}

// UILoader implementation

UILoader::UILoader() : m_base_path(".") {}

void UILoader::set_base_path(const std::string &base_path) { m_base_path = base_path; }

UILoadResult UILoader::load_from_file(const std::string &filepath, const UICallbackRegistry &callbacks) {
    UILoadResult result;

    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            result.error = "Failed to open file: " + filepath;
            return result;
        }

        json j;
        file >> j;

        return load_internal(&j, callbacks);

    } catch (const json::exception &e) {
        result.error = std::string("JSON parse error: ") + e.what();
        return result;
    } catch (const std::exception &e) {
        result.error = std::string("Error: ") + e.what();
        return result;
    }
}

UILoadResult UILoader::load_from_string(const std::string &json_str, const UICallbackRegistry &callbacks) {
    UILoadResult result;

    try {
        json j = json::parse(json_str);
        return load_internal(&j, callbacks);

    } catch (const json::exception &e) {
        result.error = std::string("JSON parse error: ") + e.what();
        return result;
    } catch (const std::exception &e) {
        result.error = std::string("Error: ") + e.what();
        return result;
    }
}

UILoadResult UILoader::load_internal(const void *json_data, const UICallbackRegistry &callbacks) {
    UILoadResult result;
    const json &j = *static_cast<const json *>(json_data);

    try {
        // Load fonts
        std::map<std::string, std::shared_ptr<Font>> fonts;
        if (j.contains("fonts")) {
            auto &font_mgr = FontManager::get();
            for (auto &[key, value] : j["fonts"].items()) {
                std::string path = value["path"].get<std::string>();
                int size = value.value("size", 18);

                // Only prepend base_path if path is relative (doesn't start with /)
                std::string full_path = path;
                if (!path.empty() && path[0] != '/') {
                    full_path = m_base_path + "/" + path;
                }

                auto font = font_mgr.load(key, full_path, size);
                if (font) {
                    fonts[key] = font;
                } else {
                    std::cerr << "Warning: Failed to load font '" << key << "' from " << full_path << std::endl;
                }
            }
        }

        // Create canvas
        int width = j.value("width", 1280);
        int height = j.value("height", 720);
        result.canvas = std::make_shared<Canvas>(width, height);

        // Load elements
        if (j.contains("elements")) {
            for (const auto &elem_json : j["elements"]) {
                std::string type = elem_json["type"].get<std::string>();
                std::string name = elem_json.value("name", "");

                std::shared_ptr<UIElement> element;

                // Create element based on type
                if (type == "Panel") {
                    auto panel = std::make_shared<Panel>(name);
                    if (elem_json.contains("color")) {
                        panel->set_color(parse_vec4(elem_json["color"]));
                    }
                    element = panel;

                } else if (type == "Text") {
                    std::string text = elem_json.value("text", "");
                    auto text_elem = std::make_shared<Text>(text, name);

                    if (elem_json.contains("font")) {
                        std::string font_name = elem_json["font"].get<std::string>();
                        if (fonts.count(font_name)) {
                            text_elem->set_font(fonts[font_name]);
                        }
                    }

                    if (elem_json.contains("color")) {
                        text_elem->set_color(parse_vec4(elem_json["color"]));
                    }

                    if (elem_json.contains("alignment")) {
                        text_elem->set_alignment(parse_text_align(elem_json["alignment"].get<std::string>()));
                    }

                    element = text_elem;

                } else if (type == "Button") {
                    std::string label = elem_json.value("label", "Button");
                    auto button = std::make_shared<Button>(label, name);

                    if (elem_json.contains("font")) {
                        std::string font_name = elem_json["font"].get<std::string>();
                        if (fonts.count(font_name)) {
                            button->get_text()->set_font(fonts[font_name]);
                        }
                    }

                    if (elem_json.contains("onClick")) {
                        std::string callback_name = elem_json["onClick"].get<std::string>();
                        auto callback = callbacks.get_callback(callback_name);
                        if (callback) {
                            button->set_on_click(callback);
                        } else {
                            std::cerr << "Warning: Callback '" << callback_name << "' not found for button '" << name
                                      << "'" << std::endl;
                        }
                    }

                    if (elem_json.contains("colors")) {
                        auto &colors = elem_json["colors"];
                        button->set_colors(parse_vec4(colors.value("normal", json::array({0.3, 0.3, 0.3, 1.0}))),
                                           parse_vec4(colors.value("hovered", json::array({0.4, 0.4, 0.4, 1.0}))),
                                           parse_vec4(colors.value("pressed", json::array({0.2, 0.2, 0.2, 1.0}))));
                    }

                    element = button;

                } else if (type == "Toggle") {
                    std::string label = elem_json.value("label", "Toggle");
                    auto toggle = std::make_shared<Toggle>(label, name);

                    if (elem_json.contains("font")) {
                        std::string font_name = elem_json["font"].get<std::string>();
                        if (fonts.count(font_name)) {
                            toggle->get_text()->set_font(fonts[font_name]);
                        }
                    }

                    if (elem_json.contains("checked")) {
                        toggle->set_checked(elem_json["checked"].get<bool>());
                    }

                    element = toggle;

                } else if (type == "Slider") {
                    float min = elem_json.value("min", 0.0f);
                    float max = elem_json.value("max", 1.0f);
                    float value = elem_json.value("value", 0.5f);
                    auto slider = std::make_shared<Slider>(min, max, value, name);

                    if (elem_json.contains("label")) {
                        slider->set_label(elem_json["label"].get<std::string>());
                    }

                    if (elem_json.contains("font")) {
                        std::string font_name = elem_json["font"].get<std::string>();
                        if (fonts.count(font_name)) {
                            slider->get_label_text()->set_font(fonts[font_name]);
                            slider->get_value_text()->set_font(fonts[font_name]);
                        }
                    }

                    if (elem_json.contains("showValue")) {
                        slider->set_show_value(elem_json["showValue"].get<bool>());
                    }

                    element = slider;

                } else {
                    std::cerr << "Warning: Unknown element type '" << type << "'" << std::endl;
                    continue;
                }

                // Set common properties
                if (element) {
                    if (elem_json.contains("position")) {
                        element->set_position(parse_vec2(elem_json["position"]));
                    }

                    if (elem_json.contains("size")) {
                        element->set_size(parse_vec2(elem_json["size"]));
                    }

                    if (elem_json.contains("anchor")) {
                        element->set_anchor(parse_anchor(elem_json["anchor"].get<std::string>()));
                    }

                    if (elem_json.contains("renderOrder")) {
                        element->set_render_order(elem_json["renderOrder"].get<int>());
                    }

                    if (elem_json.contains("visible")) {
                        element->set_visible(elem_json["visible"].get<bool>());
                    }

                    // Add to canvas
                    result.canvas->add_element(element);

                    // Add to result map
                    if (!name.empty()) {
                        result.elements[name] = element;
                    }
                }
            }
        }

        result.success = true;
        return result;

    } catch (const std::exception &e) {
        result.error = std::string("Error loading UI: ") + e.what();
        return result;
    }
}

} // namespace ui

} // namespace lmgl
