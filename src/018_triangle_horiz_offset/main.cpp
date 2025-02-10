#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

#include <util/shader.hpp>

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);

int main()
{
    GLFWwindow* window;

    // Initialize GLFW.
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr int width{ 800 };
    constexpr int height{ 600 };
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, "Learn OpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD.
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to Initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    // Setup shaders and program.
    util::Shader shader_program("shaders/vertex.vert", "shaders/fragment.frag");
    if (shader_program.error)
    {
        glfwTerminate();
        return 1;
    }

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices[] = {
        // positions                    // colors
        // Counter-clock wise.
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    // clang-format on

    // Create a VBO for positions.
    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    // Bind vertex array object.
    glBindVertexArray(vao);
    // Bind the vbo and tell VAO its memory layouts.
    // 1. Vertex positions:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // configure attrib-pointer #0 to the positions part of the vertex buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0 /* offset */);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attributes are disabled by default. We need to explicitly enable them too.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Optional: Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Little optimization to skip the other side of the triangle.
    // We are drawing the triangle in counter clockwise dir.
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CW for clock-wise

    float x_offset = 0.0;
    float sign = 1.0;
    float step = 0.01;

    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here...
        // set clear color (state setter)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        shader_program.use();
        glBindVertexArray(vao);
        shader_program.set_float("x_offset", x_offset);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Animate the triangle by moving it between the walls: x = -1.0 and x
        // = 1.0
        if (x_offset >= 0.5 || x_offset <= -0.5)
        {
            sign *= -1.0;
        }

        x_offset += (sign * step);

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// We call this in the main loop.
void process_input(GLFWwindow* window)
{
    // Returns the last reported state of a keyboard key for the specified
    // window.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
