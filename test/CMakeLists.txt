add_executable(Tests 
    core/engine_test.cpp
    
    renderer/buffer_test.cpp
    #renderer/renderer_test.cpp
    renderer/shader_test.cpp
    renderer/texture_test.cpp
    renderer/vertex_array_test.cpp

    #scene/camera_test.cpp
    #scene/light_test.cpp
    #scene/mesh_test.cpp
    #scene/node_test.cpp
    #scene/scene_test.cpp
)

target_link_libraries(Tests PRIVATE lmgl GTest::gtest_main)

if(HEADLESS)
    target_compile_definitions(Tests PRIVATE TEST_HEADLESS)
endif()

gtest_discover_tests(Tests)

