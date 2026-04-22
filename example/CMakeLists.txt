# bin
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.cc")
add_executable(mprpc_bin main.cpp ${SRC_LIST})
target_link_libraries(mprpc_bin PRIVATE mprpc)