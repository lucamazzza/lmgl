#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <initializer_list>

namespace lmgl {

namespace renderer {

/*! 
 * @enum ShaderDataType
 * @brief Enumerates the various data types used in shaders.
 *
 * This enumeration defines the different types of data that can be used
 * in shader programs, including floats, integers, matrices, and booleans.
 * Each type corresponds to a specific size and structure in memory.
 *
 * @note The sizes of these types are important for buffer layout calculations.
 */
enum class ShaderDataType {
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
};

/*! 
 * @brief Represents a single element in a buffer layout.
 *
 * This structure defines the properties of a buffer element, including its
 * name, data type, size, offset within the buffer, and whether it is normalized.
 * It is used to describe the layout of vertex attributes in a vertex buffer.
 *
 * @note The size and offset are calculated based on the ShaderDataType.
 */
struct BufferElement {

    //! @brief Name of the buffer element.
    std::string name;

    //! @brief Data type of the buffer element.
    ShaderDataType type;

    //! @brief Size of the buffer element in bytes.
    unsigned int size;

    //! @brief Offset of the buffer element within the buffer.
    size_t offset;

    //! @brief Indicates if the data is normalized.
    bool normalized;

    //! @brief Default constructor.
    BufferElement() = default;

    /*! 
     * @brief Parameterized constructor.
     *
     * Initializes a BufferElement with the specified type, name, and normalization flag.
     * Calculates the size based on the ShaderDataType.
     *
     * @param type Data type of the buffer element.
     * @param name Name of the buffer element.
     * @param normalized Indicates if the data is normalized (default is false).
     */
    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

    /*! 
     * @brief Get the number of components in the buffer element.
     *
     * This method returns the number of individual components that make up
     * the buffer element based on its ShaderDataType.
     *
     * @return Number of components in the buffer element.
     */
    unsigned int get_component_count() const;
};

/*! 
 * @brief Defines the layout of a buffer, consisting of multiple buffer elements.
 *
 * This class manages a collection of BufferElement instances, calculating
 * their offsets and the overall stride of the buffer. It provides methods
 * to access the elements and their properties.
 *
 * @note The stride is the total size of all elements in the layout.
 */
class BufferLayout {
public:

    //! @brief Default constructor.
    BufferLayout() {}
    
    /*! 
     * @brief Parameterized constructor.
     *
     * Initializes the BufferLayout with a list of BufferElement instances.
     * Calculates offsets and stride based on the provided elements.
     *
     * @param elements Initializer list of BufferElement instances.
     */
    BufferLayout(const std::initializer_list<BufferElement>& elements);

    /*! 
     * @brief Get the stride of the buffer layout.
     *
     * The stride represents the total size in bytes of all elements in the layout.
     *
     * @return Stride of the buffer layout in bytes.
     */
    inline unsigned int get_stride() const;

    /*! 
     * @brief Get the elements of the buffer layout.
     *
     * Returns a constant reference to the vector of BufferElement instances
     * that make up the layout.
     *
     * @return Constant reference to the vector of BufferElement instances.
     */
    inline const std::vector<BufferElement>& get_elements() const;

    /*! 
     * @brief Iterator support for the buffer layout elements.
     *
     * Provides begin and end iterators for both mutable and constant access
     * to the BufferElement instances in the layout.
     *
     * @return Iterators to the beginning and end of the elements vector.
     */
    std::vector<BufferElement>::iterator begin();

    /*!
     * @brief End iterator for mutable access.
     *
     * Provides an iterator to the end of the BufferElement instances
     * in the layout.
     *
     * @return End iterator for the elements vector.
     */
    std::vector<BufferElement>::iterator end();

    /*! 
     * @brief Constant iterator support for the buffer layout elements.
     *
     * Provides begin and end iterators for constant access
     * to the BufferElement instances in the layout.
     *
     * @return Constant iterators to the beginning and end of the elements vector.
     */
    std::vector<BufferElement>::const_iterator begin() const;

    /*! 
     * @brief Constant end iterator.
     *
     * Provides a constant iterator to the end of the BufferElement instances
     * in the layout.
     *
     * @return Constant end iterator for the elements vector.
     */
    std::vector<BufferElement>::const_iterator end() const;

private:

    /*! 
     * @brief Calculate offsets and stride for the buffer layout.
     *
     * This method computes the offset for each BufferElement based on
     * their sizes and updates the overall stride of the layout.
     */
    void calculate_offsets_and_stride();

    //! @brief Vector of buffer elements in the layout.
    std::vector<BufferElement> m_elements;

    //! @brief Stride of the buffer layout in bytes.
    unsigned int m_stride = 0;
};

/*! 
 * @brief Manages a vertex buffer in OpenGL.
 *
 * This class encapsulates the creation, binding, and management of a vertex buffer.
 * It allows for dynamic or static data storage and provides methods to set data
 * and retrieve the buffer layout.
 *
 * @note The buffer is identified by a renderer ID assigned by OpenGL.
 */
class VertexBuffer {
public:

    /*! 
     * @brief Constructor for the VertexBuffer.
     *
     * Initializes the vertex buffer with the provided vertex data and size.
     * The buffer can be created as dynamic or static based on the 'dynamic' flag.
     *
     * @param vertices Pointer to the vertex data.
     * @param size Size of the vertex data in bytes.
     * @param dynamic Indicates if the buffer is dynamic (default is false).
     */
    VertexBuffer(const void* vertices, unsigned int size, bool dynamic = false);

    //! @brief Destructor for the VertexBuffer.
    ~VertexBuffer();

    /*! 
     * @brief Bind the vertex buffer.
     *
     * Activates the vertex buffer for subsequent rendering operations.
     */
    void bind() const;

    /*! 
     * @brief Unbind the vertex buffer.
     *
     * Deactivates the vertex buffer.
     */
    void unbind() const;

    /*! 
     * @brief Set the data of the vertex buffer.
     *
     * Updates the contents of the vertex buffer with new data.
     *
     * @param data Pointer to the new vertex data.
     * @param size Size of the new vertex data in bytes.
     */
    void set_data(const void* data, unsigned int size);

    /*! 
     * @brief Set the layout of the vertex buffer.
     *
     * Assigns a BufferLayout to the vertex buffer, defining the structure
     * of the vertex data.
     *
     * @param layout The BufferLayout to be assigned to the vertex buffer.
     */
    void set_layout(const BufferLayout& layout);

    /*! 
     * @brief Get the layout of the vertex buffer.
     *
     * Returns a constant reference to the BufferLayout that describes
     * the structure of the vertex data in the buffer.
     *
     * @return Constant reference to the BufferLayout.
     */
    const BufferLayout& get_layout() const;

private:

    //! @brief Layout of the vertex buffer.
    unsigned int m_renderer_id;

    //! @brief Layout of the vertex buffer.
    unsigned int m_count;

    //! @brief Layout of the vertex buffer.
    BufferLayout m_layout;
};

/*! 
 * @brief Manages an index buffer in OpenGL.
 *
 * This class encapsulates the creation, binding, and management of an index buffer.
 * It stores indices used for indexed drawing and provides methods to bind and unbind
 * the buffer as well as retrieve the count of indices.
 *
 * @note The buffer is identified by a renderer ID assigned by OpenGL.
 */
class IndexBuffer {
public:

    /*! 
     * @brief Constructor for the IndexBuffer.
     *
     * Initializes the index buffer with the provided indices and count.
     *
     * @param indices Pointer to the index data.
     * @param count Number of indices.
     */
    IndexBuffer(const unsigned int* indices, unsigned int count);

    //! @brief Destructor for the IndexBuffer.
    ~IndexBuffer();

    /*! 
     * @brief Bind the index buffer.
     *
     * Activates the index buffer for subsequent rendering operations.
     */
    void bind() const;

    /*! 
     * @brief Unbind the index buffer.
     *
     * Deactivates the index buffer.
     */
    void unbind() const;

    /*! 
     * @brief Get the count of indices in the buffer.
     *
     * Returns the number of indices stored in the index buffer.
     *
     * @return Number of indices.
     */
    inline unsigned int get_count() const;

private:

    //! @brief Renderer ID assigned by OpenGL.
    unsigned int m_renderer_id;

    //! @brief Number of indices in the buffer.
    unsigned int m_count;
};

} // namespace renderer

} // namespace lmgl
