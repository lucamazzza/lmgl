add_executable(Hanoi main.cpp)
target_link_libraries(Hanoi PRIVATE lmgl)

add_executable(JSONUIDemo json_ui_demo.cpp)
target_link_libraries(JSONUIDemo PRIVATE lmgl)

add_executable(SimpleExample simple_example.cpp)
target_link_libraries(SimpleExample PRIVATE lmgl)
