cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(OpenGL VERSION 2.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
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

if (NOT TARGET glm)
    add_library(glm SHARED IMPORTED GLOBAL)
    set_target_properties(glm PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/lib/libglm.so)
    target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/include)
endif()

if (NOT TARGET util)
    add_library(util STATIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util/shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util/uniforms.cpp
    )
    target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(util PUBLIC glad -lGL glm)
endif()

if (NOT TARGET stb_image)
    add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/util/stb_image.cpp)
    target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/include)
endif()

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

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

add_executable(013_max_vertex_attribs src/013_max_vertex_attribs.cpp)
target_link_libraries(013_max_vertex_attribs PRIVATE glfw glad -lGL)

add_subdirectory(src/014_shader_vectors)

add_subdirectory(src/015_dynamic_color)

add_subdirectory(src/016_single_vbo_color)

add_subdirectory(src/017_triangle_upside_down)

add_subdirectory(src/018_triangle_horiz_offset)

add_subdirectory(src/019_triangle_position_as_color)

add_subdirectory(src/020_texture)

add_subdirectory(src/021_two_textures)

add_subdirectory(src/022_hflip_shader)

add_subdirectory(src/023_texture_wrap)

add_subdirectory(src/024_zoom_in_pixelated)

add_subdirectory(src/025_interactive_mix)

add_subdirectory(src/026_transformations)

add_subdirectory(src/027_transformations_2)

add_subdirectory(src/028_transformations_3)

add_subdirectory(src/029_transformations_4)

add_executable(001_window src/ogldev/001_window.cpp)
target_link_libraries(001_window PRIVATE glfw -lGL)

add_executable(002_point src/ogldev/002_point.cpp)
target_link_libraries(002_point PRIVATE glfw glew -lGL)

add_subdirectory(src/ogldev/004_shaders)

add_subdirectory(src/ogldev/005_uniform1f)

add_subdirectory(src/ogldev/006_translation)

add_subdirectory(src/ogldev/007_rotation)

add_subdirectory(src/ogldev/008_scaling)

add_subdirectory(src/ogldev/008.1_multi_transform)

add_subdirectory(src/ogldev/008.2_pointsize)

add_subdirectory(src/ogldev/010_indexed)

add_subdirectory(src/ogldev/011_perspective1)

add_subdirectory(src/ogldev/012_perspective2)

add_subdirectory(src/ogldev/012_perspective3)
