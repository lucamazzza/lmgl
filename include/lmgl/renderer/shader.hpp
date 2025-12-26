/*!
 * @file shader.hpp
 * @brief Declaration of the Shader class for managing shader programs in OpenGL.
 *
 * This header file contains the declaration of the Shader class, which encapsulates
 * the functionality of creating, compiling, and managing shader programs
 * in OpenGL. The class provides methods to load shader source code,
 * compile it, and set uniform variables for use in rendering.
 *
 * \copyright{Copyright (c) 2026 Luca Mazza. All rights reserved.}
 * \license{This project is released under the MIT License.}
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace lmgl {

namespace renderer {

/*!
 * @brief Represents a shader program used in rendering.
 *
 * The Shader class encapsulates the functionality to create, compile, and manage
 * shader programs consisting of vertex and fragment shaders. It provides an interface
 * for loading shader source code, compiling it, and linking it into a usable program
 * for rendering graphics.
 */
class Shader {
public:

    /*!
     * @brief Creates a shader program from vertex and fragment shader source code.
     *
     * Compiles the provided vertex and fragment shader source code, links them into a shader program,
     * and prepares it for use in rendering.
     *
     * @param vertex_src The source code for the vertex shader.
     * @param fragment_src The source code for the fragment shader.
     */
    Shader(const std::string& vert, const std::string& frag);

    /*!
     * @brief Destructor for the Shader class.
     *
     * Cleans up the shader program and releases any associated resources.
     */
    ~Shader();

    /*!
     * @brief Creates a Shader instance from vertex and fragment shader files.
     *
     * Reads the shader source code from the specified files, compiles them,
     * and links them into a shader program.
     *
     * @param vert The file path to the vertex shader source code.
     * @param frag The file path to the fragment shader source code.
     * @return A shared pointer to the created Shader instance.
     */
    static std::shared_ptr<Shader> from_vf_files(const std::string& vert, const std::string& frag);

    /*!
     * @brief Creates a Shader instance from a single GLSL file.
     *
     * Reads the shader source code from the specified GLSL file, which contains
     * both vertex and fragment shader code, compiles them, and links them into a shader program.
     *
     * @param glsl The file path to the GLSL shader source code.
     * @return A shared pointer to the created Shader instance.
     */
    static std::shared_ptr<Shader> from_glsl_file(const std::string& glsl);

    /*!
     * @brief Binds the shader program for use in rendering.
     *
     * Activates the shader program so that it is used for subsequent rendering operations.
     */
    void bind() const;

    /*!
     * @brief Unbinds the shader program.
     *
     * Deactivates the shader program, preventing it from being used in subsequent rendering operations.
     */
    void unbind() const;

    /*!
     * @brief Retrieves the unique identifier of the shader program.
     *
     * @return The OpenGL-assigned ID of the shader program.
     */
    unsigned int get_id() const;

    /*! 
     * @brief Sets an integer uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The integer value to set.
     */
    void set_int(const std::string& name, int val);

    /*! 
     * @brief Sets an array of integer uniform variables in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param vals Pointer to the array of integer values to set.
     * @param count The number of integers in the array.
     */
    void set_int_array(const std::string& name, int* vals, unsigned int count);

    /*! 
     * @brief Sets a float uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The float value to set.
     */
    void set_float(const std::string& name, float val);

    /*! 
     * @brief Sets a vec2 uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The glm::vec2 value to set.
     */
    void set_vec2(const std::string& name, const glm::vec2& val);

    /*! 
     * @brief Sets a vec3 uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The glm::vec3 value to set.
     */
    void set_vec3(const std::string& name, const glm::vec3& val);

    /*! 
     * @brief Sets a vec4 uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The glm::vec4 value to set.
     */
    void set_vec4(const std::string& name, const glm::vec4& val);

    /*! 
     * @brief Sets a mat3 uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The glm::mat3 value to set.
     */
    void set_mat3(const std::string& name, const glm::mat3& val);

    /*! 
     * @brief Sets a mat4 uniform variable in the shader program.
     *
     * @param name The name of the uniform variable.
     * @param val The glm::mat4 value to set.
     */
    void set_mat4(const std::string& name, const glm::mat4& val);

private:

    //! Caches uniform locations to optimize performance
    unsigned int m_renderer_id;

    //! Cache for uniform locations
    mutable std::unordered_map<std::string, int> m_uniform_location_cache;

    /*!
     * @brief Retrieves the location of a uniform variable in the shader program.
     *
     * Caches the location to avoid redundant OpenGL calls for performance optimization.
     *
     * @param name The name of the uniform variable.
     * @return The location of the uniform variable.
     */
    int get_uniform_location(const std::string& name) const;

    /*!
     * @brief Compiles a shader of the specified type from source code.
     *
     * Compiles the provided shader source code of the given type
     * and returns the OpenGL-assigned ID of the compiled shader.
     *
     * @param type The type of shader (e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
     * @param src The source code of the shader.
     * @return The OpenGL-assigned ID of the compiled shader.
     */
    unsigned int compile_shader(unsigned int type, const std::string& src);

    /*!
     * @brief Creates a shader program from vertex and fragment shader source code.
     *
     * Compiles the provided vertex and fragment shader source code,
     * links them into a shader program, and returns the OpenGL-assigned ID
     * of the created program.
     *
     * @param vert The source code for the vertex shader.
     * @param frag The source code for the fragment shader.
     * @return The OpenGL-assigned ID of the created shader program.
     */
    unsigned int create_program(const std::string& vert, const std::string& frag);

    /*!
     * @brief Reads the contents of a file into a string.
     *
     * Reads the entire contents of the specified file and returns it as a string.
     *
     * @param fpath The file path to read from.
     * @return The contents of the file as a string.
     */
    static std::string read_file(const std::string& fpath);
};

/*!
 * @brief Manages a collection of shader programs.
 *
 * The ShaderLibrary class provides functionality to load, store,
 * and retrieve shader programs used in rendering operations.
 * It serves as a centralized repository for shaders, allowing
 * easy access and management of multiple shader programs.
 */
class ShaderLibrary {
public:

    /*!
     * @brief Adds a shader program to the library.
     *
     * Stores the provided shader program in the library under the specified name.
     *
     * @param name The name to associate with the shader program.
     * @param shader A shared pointer to the Shader instance to add.
     */
    static void add(const std::string& name, const std::shared_ptr<Shader> shader);

    /*!
     * @brief Loads a shader program from vertex and fragment shader files and adds it to the library.
     *
     * Reads the shader source code from the specified files, compiles them,
     * links them into a shader program, and stores it in the library under the given name.
     *
     * @param name The name to associate with the shader program.
     * @param vert The file path to the vertex shader source code.
     * @param frag The file path to the fragment shader source code.
     * @return A shared pointer to the created Shader instance.
     */
    static std::shared_ptr<Shader> load(const std::string& name, const std::string& vert, const std::string& frag);

    /*!
     * @brief Retrieve a shader program from the library by name.
     *
     * Returns the shader program associated with the specified name.
     *
     * @param name The name of the shader program to retrieve.
     * @return A shared pointer to the Shader instance.
     */
    static std::shared_ptr<Shader> get(const std::string& name);

    /*!
     * @brief Checks if a shader program exists in the library by name.
     *
     * @param name The name of the shader program to check.
     * @return True if the shader program exists, false otherwise.
     */
    static bool exists(const std::string& name);

    /*!
     * @brief Clears all shader programs from the library.
     *
     * Removes all stored shader programs, freeing associated resources.
     */
    static void clear();

private:

    //! Static map storing shader programs by name
    static std::unordered_map<std::string, std::shared_ptr<Shader>> s_shaders;
};

} // namespace renderer

} // namespace lmgl
