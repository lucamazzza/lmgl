cmake_minimum_required(VERSION 3.10)
project(MyGraphicsEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(MyEngine STATIC 
    # Add all your source files here
)

target_include_directories(MyEngine PUBLIC include)

target_link_libraries(MyEngine PUBLIC glfw ${OPENGL_LIBRARIES})

if(APPLE)
    target_link_libraries(MyEngine PUBLIC "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

add_executable(Sandbox sandbox/main.cpp)
target_link_libraries(Sandbox PRIVATE MyEngine)
