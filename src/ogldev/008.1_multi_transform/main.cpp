#include "util/uniforms.hpp"
#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

#include <util/shader.hpp>

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);
static void create_buffers(GLuint& vao);

struct Buffers
{
    GLuint vao;
    GLuint vbo;
    Buffers(const float* vertices, size_t num_verts);
    ~Buffers();
};

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
    constexpr int height{ 800 };
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

    util::Matrix4f combined_mat("combined_mat");

    // Setup shaders and program.
    util::Shader shader_program("shaders/vertex.vert", "shaders/fragment.frag", { &combined_mat });
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

    {
        Buffers bufs(vertices, 3);

        // Little optimization to skip the other side of the triangle.
        // We are drawing the triangle in counter-clockwise dir.
        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK); // cull back face
        glFrontFace(GL_CCW); // GL_CW for clock-wise

        util::Mat4x4f tran_mat{
            // row 0
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            // row 1
            0.0f,
            1.0f,
            0.0f,
            0.0f,
            // row 2
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            // row 3
            0.0f,
            0.0f,
            0.0f,
            1.0f,
        };

        float scale = 0.5f;
        util::Mat4x4f scale_mat{
            // row 0
            scale,
            0.0f,
            0.0f,
            0.0f,
            // row 1
            0.0f,
            scale,
            0.0f,
            0.0f,
            // row 2
            0.0f,
            0.0f,
            scale,
            0.0f,
            // row 3
            0.0f,
            0.0f,
            0.0f,
            1.0f,
        };

        util::Mat4x4f tr_scl_mat = scale_mat * tran_mat;
        float angle = 0.0f;
        float delta = 0.03f;
        util::Mat4x4f rot_mat;
        for (int ii = 0; ii < 4; ++ii)
            for (int jj = 0; jj < 4; ++jj)
                rot_mat.mat[ii][ii] = (ii == jj) ? 1.0f : 0.0f;

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

            if (angle > 3.1415f || angle < -3.1415f)
            {
                delta = -delta;
            }
            angle += delta;
            auto& mat = rot_mat.mat;
            mat[0][0] = std::cos(angle);
            mat[0][1] = -std::sin(angle);
            mat[1][0] = std::sin(angle);
            mat[1][1] = std::cos(angle);
            util::Mat4x4f res = rot_mat * tr_scl_mat;
            // set the uniform variable 4x4 matrix.
            combined_mat.set(res);

            glBindVertexArray(bufs.vao);

            glDrawArrays(GL_TRIANGLES, 0, 3);
            // No need to unbind it every time.
            // glBindVertexArray(0);

            // poll and process events
            glfwPollEvents();
            // swap buffers
            glfwSwapBuffers(window);
        }
    }

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

Buffers::Buffers(const float* vertices, size_t num_verts)
{
    constexpr size_t vert_dims = 3; // x, y, z or r, b, g
    constexpr size_t attributes = 2; // pos + colors.

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (num_verts * vert_dims * attributes), vertices,
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    // Bind vertex array object.
    glBindVertexArray(vao);
    // Bind the vbo and tell VAO its memory layouts.
    // 1. Vertex positions:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // configure attrib-pointer #0 to the positions part of the vertex buffer.
    glVertexAttribPointer(0, vert_dims, GL_FLOAT, GL_FALSE,
                          (vert_dims * attributes) * sizeof(float), (void*)0 /* offset */);
    // configure attrib-pointer #1 to the color part of the vertex buffer.
    glVertexAttribPointer(1, vert_dims, GL_FLOAT, GL_FALSE,
                          (vert_dims * attributes) * sizeof(float),
                          (void*)(vert_dims * sizeof(float)));

    // Attributes are disabled by default. We need to explicitly enable them too.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Optional: Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Buffers::~Buffers()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
