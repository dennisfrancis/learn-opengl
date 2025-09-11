set(PROJECT_NAME 007_rotation)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glew -lGL util)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME main)

add_custom_target(
    ${PROJECT_NAME}.shaders
    ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/shaders
    COMMENT "Copying Files for target: ${PROJECT_NAME}"
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}.shaders)
