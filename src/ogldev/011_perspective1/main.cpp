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

    void set(float x_, float y_, float z_)
    {
        x = x_;
        y = y_;
        z = z_;
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

struct FrameContext
{
    float& angle;
    float& delta;
    util::Mat4x4f& perspective;
    util::Mat4x4f& translation;
    util::Mat4x4f& rotation;
    util::Matrix4f& total_transform;
};

void display_frame(GLFWwindow* window, Buffers& bufs, size_t num_indices,
                   util::Shader& shader_program, FrameContext& ctxt)
{
    float& angle = ctxt.angle;
    float& delta = ctxt.delta;
    util::Mat4x4f& perspective = ctxt.perspective;
    util::Mat4x4f& translation = ctxt.translation;
    util::Mat4x4f& rotation = ctxt.rotation;
    util::Matrix4f& total_transform = ctxt.total_transform;
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

    float (&rmat)[4][4] = rotation.mat;
    float (&tmat)[4][4] = translation.mat;

    // rotation animation.
    rmat[0][0] = std::cos(angle);
    rmat[0][2] = -std::sin(angle);
    rmat[2][0] = std::sin(angle);
    rmat[2][2] = std::cos(angle);
    // calculate the final transformation.
    total_transform.set(perspective * translation * rotation);

    glBindVertexArray(bufs.vao);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, 0);
    // No need to unbind it every time.
    // glBindVertexArray(0);

    // poll and process events
    glfwPollEvents();
    // swap buffers
    glfwSwapBuffers(window);
}

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

    util::Matrix4f trans_tot("trans_tot");

    // Setup shaders and program.
    util::Shader shader_program("shaders/vertex.vert", "shaders/fragment.frag", { &trans_tot });
    if (shader_program.error)
    {
        glfwTerminate();
        return 1;
    }

    {
        constexpr size_t num_verts = 8;
        constexpr size_t num_indices = 12 * 3;
        ColoredVertex vertices[num_verts];
        {
            vertices[0].set(-0.5f, -0.5f, 0.5f);
            vertices[1].set(-0.5f, 0.5f, 0.5f);
            vertices[2].set(-0.5f, -0.5f, -0.5f);
            vertices[3].set(-0.5f, 0.5f, -0.5f);
            vertices[4].set(0.5f, -0.5f, 0.5f);
            vertices[5].set(0.5f, 0.5f, 0.5f);
            vertices[6].set(0.5f, -0.5f, -0.5f);
            vertices[7].set(0.5f, 0.5f, -0.5f);
        }

        uint16_t indices[num_indices] = {
            // tr1
            1,
            2,
            0,
            // tr2
            3,
            6,
            2,
            // tr3
            7,
            4,
            6,
            // tr4
            5,
            0,
            4,
            // tr5
            6,
            0,
            2,
            // tr6
            3,
            5,
            7,
            // tr7
            1,
            3,
            2,
            // tr8
            3,
            7,
            6,
            // tr9
            7,
            5,
            4,
            // tr10
            5,
            1,
            0,
            // tr11
            6,
            4,
            0,
            // tr12
            3,
            1,
            5,
        };
        Buffers bufs(vertices, indices, num_verts, num_indices);

        // Little optimization to skip the other side of the triangle.
        // We are drawing the triangle in counter-clockwise dir.
        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK); // cull back face
        glFrontFace(GL_CW); // GL_CW for clock-wise

        util::Mat4x4f rot;
        util::Mat4x4f translation;
        float (&rmat)[4][4] = rot.mat;
        float (&tmat)[4][4] = translation.mat;
        float angle = 0.0f;
        float delta = 0.03f;
        for (int ii = 0; ii < 4; ++ii)
        {
            for (int jj = 0; jj < 4; ++jj)
            {
                tmat[ii][jj] = rmat[ii][jj] = (ii == jj) ? 1.0f : 0.0f;
            }
        }

        tmat[2][3] = 2.5f;

        float FOV = 90.0f; // in degrees.
        float tanHalfFOV = std::tan(FOV * M_PI / 360.0);
        float d = 1 / tanHalfFOV;

        util::Mat4x4f perspective{
            // row 0
            d,
            0.0f,
            0.0f,
            0.0f,
            // row 1
            0.0f,
            d,
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
            1.0f,
            0.0f,
        };

        FrameContext ctxt{ angle, delta, perspective, translation, rot, trans_tot };

        while (!glfwWindowShouldClose(window))
        {
            display_frame(window, bufs, num_indices, shader_program, ctxt);
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
