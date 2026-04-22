file(GLOB_RECURSE SRC_LIST "*.cpp" "*.cc")

add_executable(test_bin main.cpp ${SRC_LIST})

target_link_libraries(test_bin PRIVATE mprpc)
