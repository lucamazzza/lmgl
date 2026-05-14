add_executable(Hanoi main.cpp)
target_link_libraries(Hanoi PRIVATE lmgl)

add_custom_command(TARGET Hanoi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:Hanoi>/shaders"
    COMMENT "Copying shader directory..."
)

add_custom_command(TARGET Hanoi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/examples/assets"
        "$<TARGET_FILE_DIR:Hanoi>/examples/assets"
    COMMENT "Copying assets directory..."
)

if(WIN32 AND LMGL_ENABLE_OVR)
  add_custom_command(TARGET Hanoi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/openvr/bin/win64/openvr_api.dll"
        "$<TARGET_FILE_DIR:Hanoi>")
endif()

if(WIN32 AND LMGL_ENABLE_LEAP_MOTION)
  add_custom_command(TARGET Hanoi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/leap/bin/x64/LeapC.dll"
        "$<TARGET_FILE_DIR:Hanoi>")
endif()

add_executable(JSONUIDemo json_ui_demo.cpp)
target_link_libraries(JSONUIDemo PRIVATE lmgl)

add_executable(SimpleExample simple_example.cpp)
target_link_libraries(SimpleExample PRIVATE lmgl)
