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

if (NOT TARGET glad)
    add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c)
    target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include)
endif()

add_executable(001_triangle 001_triangle.cpp)
target_link_libraries(001_triangle PRIVATE glfw -lGL)

add_executable(002_glew 002_glew.cpp)
target_link_libraries(002_glew PRIVATE glfw glew -lGL)

add_executable(003_vertex_buffer 003_vertex_buffer.cpp)
target_link_libraries(003_vertex_buffer PRIVATE glfw glew -lGL)

add_executable(004_glad 004_glad.cpp)
target_link_libraries(004_glad PRIVATE glfw glad -lGL)

add_executable(005_keyinput 005_keyinput.cpp)
target_link_libraries(005_keyinput PRIVATE glfw glad -lGL)
