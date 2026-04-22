cmake_minimum_required(VERSION 3.10.0)
project(mprpcs VERSION 0.1.0 LANGUAGES C CXX)


# 配置
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/out)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # -fPIC，动态库必需
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options($<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-O2>)


# 依赖
find_package(muduo REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PROTOBUF REQUIRED IMPORTED_TARGET protobuf)


add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(test)