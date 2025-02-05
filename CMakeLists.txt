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

add_executable(001_triangle src/001_triangle.cpp)
target_link_libraries(001_triangle PRIVATE glfw -lGL)

add_executable(002_glew src/002_glew.cpp)
target_link_libraries(002_glew PRIVATE glfw glew -lGL)

add_executable(003_vertex_buffer src/003_vertex_buffer.cpp)
target_link_libraries(003_vertex_buffer PRIVATE glfw glew -lGL)

add_executable(004_glad src/004_glad.cpp)
target_link_libraries(004_glad PRIVATE glfw glad -lGL)

add_executable(005_keyinput src/005_keyinput.cpp)
target_link_libraries(005_keyinput PRIVATE glfw glad -lGL)

add_executable(006_window_clear src/006_window_clear.cpp)
target_link_libraries(006_window_clear PRIVATE glfw glad -lGL)

add_executable(007_triangle src/007_triangle.cpp)
target_link_libraries(007_triangle PRIVATE glfw glad -lGL)

add_executable(008_rectangle src/008_rectangle.cpp)
target_link_libraries(008_rectangle PRIVATE glfw glad -lGL)

add_executable(009_triangle_color_interpolate src/009_triangle_color_interpolate.cpp)
target_link_libraries(009_triangle_color_interpolate PRIVATE glfw glad -lGL)

add_executable(010_two_triangles src/010_two_triangles.cpp)
target_link_libraries(010_two_triangles PRIVATE glfw glad -lGL)

add_executable(011_triangles_two_vao_vbo src/011_triangles_two_vao_vbo.cpp)
target_link_libraries(011_triangles_two_vao_vbo PRIVATE glfw glad -lGL)

add_executable(012_two_triangles_different_colors src/012_two_triangles_different_colors.cpp)
target_link_libraries(012_two_triangles_different_colors PRIVATE glfw glad -lGL)
