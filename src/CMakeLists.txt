# 动态库
file(GLOB_RECURSE SRC_LIST "*.cpp" "*.cc")
add_library(mprpc SHARED ${SRC_LIST})
target_link_libraries(mprpc PUBLIC 
    pthread 
    PkgConfig::PROTOBUF
    muduo
    grpc
    grpc++
    gpr
)
target_include_directories(mprpc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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


