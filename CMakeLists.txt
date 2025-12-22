cmake_minimum_required(VERSION 3.10)
project(lmgl VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HEADLESS "Build without display support for CI" OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/googletest)

find_package(OpenGL REQUIRED)

add_library(lmgl STATIC 
    # core
    include/lmgl/core/engine.hpp
    src/core/engine.cpp

    # renderer
    include/lmgl/renderer/buffer.hpp
    #include/lmgl/renderer/renderer.hpp
    #include/lmgl/renderer/shader.hpp
    #include/lmgl/renderer/texture.hpp
    include/lmgl/renderer/vertex_array.hpp
    src/renderer/buffer.cpp
    #src/renderer/renderer.cpp
    #src/renderer/shader.cpp
    #src/renderer/texture.cpp
    src/renderer/vertex_array.cpp

    # scene
    #include/lmgl/scene/camera.hpp
    #include/lmgl/scene/light.hpp
    #include/lmgl/scene/mesh.hpp
    #include/lmgl/scene/node.hpp
    #include/lmgl/scene/scene.hpp
    #src/scene/camera.cpp
    #src/scene/light.cpp
    #src/scene/mesh.cpp
    #src/scene/node.cpp
    #src/scene/scene.cpp
)

target_include_directories(lmgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(lmgl PUBLIC external/glad/include)

if(HEADLESS)
    target_compile_definitions(lmgl PRIVATE TEST_HEADLESS)
endif()

target_sources(lmgl PRIVATE external/glad/src/glad.c)

target_link_libraries(lmgl PUBLIC glfw glm::glm ${OPENGL_LIBRARY})

if(APPLE)
    target_link_libraries(lmgl PUBLIC "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

add_subdirectory(sandbox)

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
