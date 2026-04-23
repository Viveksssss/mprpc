include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_libraries(mprpc)
add_subdirectory(callee)
add_subdirectory(caller)