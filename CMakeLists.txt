cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(OpenGL VERSION 2.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if (NOT TARGET glfw)
    add_library(glfw SHARED IMPORTED GLOBAL)
    set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib/libglfw.so)
    target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)
endif()

if (NOT TARGET glew)
    add_library(glew SHARED IMPORTED GLOBAL)
    set_target_properties(glew PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/lib64/libGLEW.so)
    target_include_directories(glew INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/include)
endif()

add_executable(01_triangle 01_triangle.cpp)
target_link_libraries(01_triangle PRIVATE glfw -lGL)

add_executable(02_glew 02_glew.cpp)
target_link_libraries(02_glew PRIVATE glfw glew -lGL)

add_executable(03_vertex_buffer 03_vertex_buffer.cpp)
target_link_libraries(03_vertex_buffer PRIVATE glfw glew -lGL)
