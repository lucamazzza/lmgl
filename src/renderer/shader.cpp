#include "lmgl/renderer/shader.hpp"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>

namespace lmgl {

namespace renderer {

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_shaders;

// Shader

Shader::Shader(const std::string &vert, const std::string &frag) {
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vert);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, frag);
    m_renderer_id = create_program(vertex_shader, fragment_shader);
}

Shader::Shader(const std::string &vert, const std::string &geom, const std::string &frag) {
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vert);
    unsigned int geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geom);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, frag);
    m_renderer_id = create_program(vertex_shader, geometry_shader, fragment_shader);
}

Shader::~Shader() { glDeleteProgram(m_renderer_id); }

std::shared_ptr<Shader> Shader::from_vf_files(const std::string &vert, const std::string &frag) {
    std::string vertex_src = read_file(vert);
    std::string fragment_src = read_file(frag);
    return std::make_shared<Shader>(vertex_src, fragment_src);
}

std::shared_ptr<Shader> Shader::from_vgf_files(const std::string &vert, const std::string &geom, const std::string &frag) {
    std::string vertex_src = read_file(vert);
    std::string geometry_src = read_file(geom);
    std::string fragment_src = read_file(frag);
    return std::make_shared<Shader>(vertex_src, geometry_src, fragment_src);
}

std::shared_ptr<Shader> Shader::from_glsl_file(const std::string &glsl) {
    std::string src = read_file(glsl);
    auto shaders = parse_glsl_shader(src);
    if (shaders.size() == 2) {
        return std::make_shared<Shader>(shaders[0], shaders[1]);
    } else if (shaders.size() == 3) {
        return std::make_shared<Shader>(shaders[0], shaders[1], shaders[2]);
    }
    std::cerr << "ERROR: Invalid shader file format" << std::endl;
    return nullptr;
}

void Shader::bind() const { glUseProgram(m_renderer_id); }

void Shader::unbind() const { glUseProgram(0); }

unsigned int Shader::get_id() const { return m_renderer_id; }

void Shader::set_int(const std::string &name, int val) { glUniform1i(get_uniform_location(name), val); }

void Shader::set_int_array(const std::string &name, int *vals, unsigned int count) {
    glUniform1iv(get_uniform_location(name), count, vals);
}

void Shader::set_float(const std::string &name, float val) { glUniform1f(get_uniform_location(name), val); }

void Shader::set_vec2(const std::string &name, const glm::vec2 &val) {
    glUniform2f(get_uniform_location(name), val.x, val.y);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &val) {
    glUniform3f(get_uniform_location(name), val.x, val.y, val.z);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &val) {
    glUniform4f(get_uniform_location(name), val.x, val.y, val.z, val.w);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &val) {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &val) {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &val[0][0]);
}

int Shader::get_uniform_location(const std::string &name) const {
    if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end()) {
        return m_uniform_location_cache[name];
    }
    int loc = glGetUniformLocation(m_renderer_id, name.c_str());
    if (loc == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    } else {
        m_uniform_location_cache[name] = loc;
    }
    return loc;
}

unsigned int Shader::compile_shader(unsigned int type, const std::string &source) {
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        char *msg = new char[len];
        glGetShaderInfoLog(shader, len, &len, msg);
        const char* shader_type = 
            type == GL_VERTEX_SHADER ? "VERTEX" :
            type == GL_GEOMETRY_SHADER ? "GEOMETRY" :
            type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "UNKNOWN";
        std::cerr << "ERROR: Shader compilation failed (" << shader_type << "):" << std::endl;
        std::cerr << msg << std::endl;
        delete[] msg;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

unsigned int Shader::create_program(unsigned int vert, unsigned int frag) {
    if (vert == 0 || frag == 0)
        return 0;
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        char *msg = new char[len];
        glGetProgramInfoLog(program, len, &len, msg);
        std::cerr << "ERROR: Shader program linking failed:" << std::endl;
        std::cerr << msg << std::endl;
        delete[] msg;
        glDeleteProgram(program);
        return 0;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

unsigned int Shader::create_program(unsigned int vert, unsigned int geom, unsigned int frag) {
    if (vert == 0 || geom == 0 || frag == 0)
        return 0;
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, geom);
    glAttachShader(program, frag);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        char *msg = new char[len];
        glGetProgramInfoLog(program, len, &len, msg);
        std::cerr << "ERROR: Shader program linking failed:" << std::endl;
        std::cerr << msg << std::endl;
        delete[] msg;
        glDeleteProgram(program);
        return 0;
    }
    glDeleteShader(vert);
    glDeleteShader(geom);
    glDeleteShader(frag);
    return program;
}

std::string Shader::read_file(const std::string &fpath) {
    std::ifstream file(fpath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file: " << fpath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string> Shader::parse_glsl_shader(const std::string &src) {
    enum class ShaderType { NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2 };
    std::stringstream ss(src);
    std::string line;
    std::stringstream shaders[3];
    ShaderType type = ShaderType::NONE;
    bool has_geometry = false;
    while (std::getline(ss, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("geometry") != std::string::npos) {
                type = ShaderType::GEOMETRY;
                has_geometry = true;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            if (type != ShaderType::NONE) {
                shaders[static_cast<int>(type)] << line << '\n';
            }
        }
    }
    if (has_geometry) {
        return {shaders[0].str(), shaders[1].str(), shaders[2].str()};
    }
    return {shaders[0].str(), shaders[2].str()};
}

// ShaderLibrary

void ShaderLibrary::add(const std::string &name, std::shared_ptr<Shader> shader) {
    if (exists(name)) {
        std::cerr << "Warning: Shader '" << name << "' already exists in the library!" << std::endl;
    }
    if (shader == nullptr || shader->get_id() == 0) {
        std::cerr << "Error: Cannot add invalid shader '" << name << "' to the library!" << std::endl;
        return;
    }
    s_shaders[name] = shader;
}

std::shared_ptr<Shader> ShaderLibrary::load_vf(const std::string &name, const std::string &vert,
                                               const std::string &frag) {
    auto shader = Shader::from_vf_files(vert, frag);
    add(name, shader);
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::load_glsl(const std::string &name, const std::string &src) {
    auto shader = Shader::from_glsl_file(src);
    add(name, shader);
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::get(const std::string &name) {
    if (!exists(name)) {
        std::cerr << "Error: Shader '" << name << "' not found in the library!" << std::endl;
        return nullptr;
    }
    return s_shaders[name];
}

bool ShaderLibrary::exists(const std::string &name) { return s_shaders.find(name) != s_shaders.end(); }

void ShaderLibrary::clear() { s_shaders.clear(); }

} // namespace renderer

} // namespace lmgl
