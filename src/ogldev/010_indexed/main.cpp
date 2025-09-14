#include "util/uniforms.hpp"
#include <cmath>
#include <cstdint>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>
#include <random>

#include <util/shader.hpp>

static std::random_device rd; // Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
static std::uniform_real_distribution<> unirand(0.0, 1.0);

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);
static void create_buffers(GLuint& vao);

struct ColoredVertex
{
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    ColoredVertex() {}
    void set(float x_, float y_)
    {
        x = x_;
        y = y_;
        z = 0.0f;
        r = static_cast<float>(unirand(gen));
        g = static_cast<float>(unirand(gen));
        b = static_cast<float>(unirand(gen));
    }
};

struct Buffers
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    Buffers(const ColoredVertex* vertices, const uint16_t* indices, size_t num_verts,
            size_t num_indices);
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

    constexpr int width{ 600 };
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

    util::Matrix4f rot_mat("rot_mat");

    // Setup shaders and program.
    util::Shader shader_program("shaders/vertex.vert", "shaders/fragment.frag", { &rot_mat });
    if (shader_program.error)
    {
        glfwTerminate();
        return 1;
    }

    constexpr size_t num_verts = 19;
    constexpr size_t num_indices = 54;
    ColoredVertex vertices[num_verts];
    {
        // Center
        vertices[0].set(0.0f, 0.0);

        // Top row
        vertices[1].set(-1.0f, 1.0f);
        vertices[2].set(-0.75f, 1.0f);
        vertices[3].set(-0.50f, 1.0f);
        vertices[4].set(-0.25f, 1.0f);
        vertices[5].set(-0.0f, 1.0f);
        vertices[6].set(0.25f, 1.0f);
        vertices[7].set(0.50f, 1.0f);
        vertices[8].set(0.75f, 1.0f);
        vertices[9].set(1.0f, 1.0f);

        // Bottom row
        vertices[10].set(-1.0f, -1.0f);
        vertices[11].set(-0.75f, -1.0f);
        vertices[12].set(-0.50f, -1.0f);
        vertices[13].set(-0.25f, -1.0f);
        vertices[14].set(-0.0f, -1.0f);
        vertices[15].set(0.25f, -1.0f);
        vertices[16].set(0.50f, -1.0f);
        vertices[17].set(0.75f, -1.0f);
        vertices[18].set(1.0f, -1.0f);
    }

    uint16_t indices[num_indices] = {

        // Top triangles
        // 1
        0, 2, 1,
        // 2
        0, 3, 2,
        // 3
        0, 4, 3,
        // 4
        0, 5, 4,
        // 5
        0, 6, 5,
        // 6
        0, 7, 6,
        // 7
        0, 8, 7,
        // 8
        0, 9, 8,

        // Bottom triangles
        // 9
        0, 10, 11,
        // 10
        0, 11, 12,
        // 11
        0, 12, 13,
        // 12
        0, 13, 14,
        // 13
        0, 14, 15,
        // 14
        0, 15, 16,
        // 15
        0, 16, 17,
        // 16
        0, 17, 18,

        // Left triangle
        // 17
        0, 1, 10,

        // Right triangle
        // 18
        0, 18, 9
    };

    {
        Buffers bufs(vertices, indices, num_verts, num_indices);

        // Little optimization to skip the other side of the triangle.
        // We are drawing the triangle in counter-clockwise dir.
        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK); // cull back face
        glFrontFace(GL_CCW); // GL_CW for clock-wise

        // Initialize the translation matrix:
        float (&mat)[4][4] = rot_mat.get();
        float angle = 0.0f;
        float delta = 0.03f;
        for (int ii = 0; ii < 4; ++ii)
        {
            for (int jj = 0; jj < 4; ++jj)
            {
                mat[ii][jj] = (ii == jj) ? 1.0f : 0.0f;
            }
        }

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
            // set the uniform variable 4x4 matrix.
            mat[0][0] = 0.5f * std::cos(angle);
            mat[0][1] = 0.5f * -std::sin(angle);
            mat[1][0] = 0.5f * std::sin(angle);
            mat[1][1] = 0.5f * std::cos(angle);
            rot_mat.set();

            glBindVertexArray(bufs.vao);

            glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0);
            // No need to unbind it every time.
            // glBindVertexArray(0);

            // poll and process events
            glfwPollEvents();
            // swap buffers
            glfwSwapBuffers(window);
        }
    }

    glfwDestroyWindow(window);
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

Buffers::Buffers(const ColoredVertex* vertices, const uint16_t* indices, size_t num_verts,
                 size_t num_indices)
{
    glGenVertexArrays(1, &vao);
    // Bind vertex array object.
    glBindVertexArray(vao);

    // Bind the vbo and tell VAO its memory layouts.
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredVertex) * num_verts, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // configure attrib-pointer #0 to the positions part of the vertex buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0 /* offset */);
    // configure attrib-pointer #1 to the color part of the vertex buffer.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attributes are disabled by default. We need to explicitly enable them too.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * num_indices, indices, GL_STATIC_DRAW);

    // Optional: Unbind VAO, VBO and IBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Buffers::~Buffers()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}
