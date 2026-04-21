cmake_minimum_required(VERSION 3.10.0)
project(mprpc VERSION 0.1.0 LANGUAGES C CXX)


# 配置
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # -fPIC，动态库必需
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-g -O2)


# 依赖
find_package(muduo REQUIRED)


# 动态库
file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.cc")
add_library(mprpc SHARED ${SRC_LIST})
target_link_libraries(mprpc pthread)
target_include_directories(mprpc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)


install(DIRECTORY include/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h *.hpp"
)

install(TARGETS mprpc 
    EXPORT mprpcTargets
    LIBRARY DESTINATION lib
)

install(EXPORT mprpcTargets
    FILE mprpcTargets.cmake
    DESTINATION lib/cmake/mprpc
)






# bin
add_executable(mprpc_bin main.cpp)
target_link_libraries(mprpc_bin PRIVATE muduo)
