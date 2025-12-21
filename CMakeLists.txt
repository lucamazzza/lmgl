cmake_minimum_required(VERSION 3.10)
project(lmgl VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/googletest)

find_package(OpenGL REQUIRED)

add_library(lmgl STATIC 
    # core
    include/lgml/core/window.hpp
    src/core/window.cpp

    # renderer
    include/lgml/renderer/buffer.hpp
    include/lgml/renderer/renderer.hpp
    include/lgml/renderer/shader.hpp
    include/lgml/renderer/texture.hpp
    include/lgml/renderer/vertex_array.hpp
    src/renderer/renderer.cpp

    # scene
    include/lgml/scene/camera.hpp
    include/lgml/scene/light.hpp
    include/lgml/scene/mesh.hpp
    include/lgml/scene/node.hpp
    include/lgml/scene/scene.hpp
    src/scene/scene.cpp
)

target_include_directories(lmgl PUBLIC include)
target_include_directories(lmgl PUBLIC external/glad/include)

target_sources(lmgl PRIVATE external/glad/src/glad.c)

target_link_libraries(lmgl PUBLIC glfw glm::glm ${OPENGL_LIBRARY})

if(APPLE)
    target_link_libraries(lmgl PUBLIC "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

add_executable(Sandbox sandbox/main.cpp)
target_link_libraries(Sandbox PRIVATE lmgl)

set(CPACK_PACKAGE_NAME "lmgl")
set(CPACK_PACKAGE_VENDOR "Luca Mazza")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "lmgl Graphics Engine Library")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)

enable_testing()
include(GoogleTest)
add_subdirectory(test)
