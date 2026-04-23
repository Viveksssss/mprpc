# bin
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.cc")

add_compile_options(-g)

add_executable(callee ${SRC_LIST} ../user.pb.cc)
target_link_libraries(callee PRIVATE mprpc)