cmake_minimum_required(VERSION 3.10)
project(MyGraphicsEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(MyEngine STATIC 
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

target_include_directories(MyEngine PUBLIC include)

target_link_libraries(MyEngine PUBLIC glfw ${OPENGL_LIBRARIES})

if(APPLE)
    target_link_libraries(MyEngine PUBLIC "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

add_executable(Sandbox sandbox/main.cpp)
target_link_libraries(Sandbox PRIVATE MyEngine)
