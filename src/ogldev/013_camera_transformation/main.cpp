// This sample shows how to change the projection matrix so the final zmin and
// zmax are in (-1.0, 1.0) where there is highest precision for float
// variables.
//
// This is especially important in z direction as z coordinate is used for
// depth test in the depth buffer to see which pixels are occluding other
// pixels.
//
// This done by defining A & B in the projection matrix with respect to the
// z_near and z_far we want.

#include "util/3dtypes.hpp"
#include "util/uniforms.hpp"
#include <cmath>
#include <cstdint>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

#include <util/shader.hpp>

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
        set_rgb();
    }

    void set(float x_, float y_, float z_)
    {
        x = x_;
        y = y_;
        z = z_;
        set_rgb();
    }

private:
    void set_rgb()
    {
        r = static_cast<float>(util::random_float());
        g = static_cast<float>(util::random_float());
        b = static_cast<float>(util::random_float());
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
    util::Mat4x4f& camera; // camera or view transformation.
    util::Mat4x4f& translation;
    util::Mat4x4f& rotation;
    util::Matrix4f& WVP; // world view projection transformation (combined).
};

void display_frame(GLFWwindow* window, Buffers& bufs, size_t num_indices,
                   util::Shader& shader_program, FrameContext& ctxt)
{
    float& angle = ctxt.angle;
    float& delta = ctxt.delta;
    util::Mat4x4f& perspective = ctxt.perspective;
    util::Mat4x4f& camera = ctxt.camera;
    util::Mat4x4f& translation = ctxt.translation;
    util::Mat4x4f& rotation = ctxt.rotation;
    util::Matrix4f& WVP = ctxt.WVP;
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
    // translation * rotation is the world transformation.
    WVP.set(perspective * camera * translation * rotation);

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

    constexpr int width{ 1200 };
    constexpr int height{ 900 };

    float ar = static_cast<float>(width) / height;
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

    util::Matrix4f WVP("wvp");

    // Setup shaders and program.
    util::Shader shader_program("shaders/vertex.vert", "shaders/fragment.frag", { &WVP });
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

        // translate the cube a bit away from the origin in the z-direction so
        // it is fully in the view frustum.
        tmat[2][3] = 2.0f;

        // Camera transformation:
        // Camera uses a U, V, N model.

        // camera moves back in z so the cube looks smaller.
        // float camera_pos[3] = { 0.0f, 0.0f, -1.0f }; 

        // camera moves right (along x axis), so the cube moves to the left.
        // float camera_pos[3] = { 1.0f, 0.0f, 0.0f }; 

        // camera moves up(y axis) so now we can see the cube's top side.
        // float camera_pos[3] = { 0.0f, 0.9f, 0.0f }; 

        // camera moves down(y axis) so now we can see the cube's bottom side.
        float camera_pos[3] = { 0.0f, -0.9f, 0.0f }; 

        // These have to be normalized.
        float camera_U[3] = { 1.0f, 0.0f, 0.0f };
        float camera_V[3] = { 0.0f, 1.0f, 0.0f };
        float camera_N[3] = { 0.0f, 0.0f, 1.0f };
        util::Mat4x4f camera_translation{

            // row 0
            1.0f, 0.0f, 0.0f, -camera_pos[0],
            // row 1
            0.0f, 1.0f, 0.0f, -camera_pos[1],
            // row 2
            0.0f, 0.0f, 1.0f, -camera_pos[2],
            // row 3
            0.0f, 0.0f, 0.0f, 1.0f
        };
        util::Mat4x4f camera_rotation{
            
            // row 0
            camera_U[0], camera_U[1], camera_U[2], 0.0f,
            // row 1
            camera_V[0], camera_V[1], camera_V[2], 0.0f,
            // row 2
            camera_N[0], camera_N[1], camera_N[2], 0.0f,
            // row 3
            0.0f, 0.0f, 0.0f, 1.0f
        };

        util::Mat4x4f camera_transformation = camera_rotation * camera_translation;

        // Perspective projection matrix.

        // same FOV is used for the vertical FOV and horizontal FOV.
        float FOV = 90.0f; // in degrees.
        float tanHalfFOV = std::tan(FOV * M_PI / 360.0);
        // Assumes the near clip-plane is at z = 1.0;
        float d = 1 / tanHalfFOV;

        // Change near_z and far_z to see the clipping.
        const float near_z = 1.0f;
        const float far_z = 10.0f;
        const float z_range = near_z - far_z;
        const float A = (-far_z - near_z) / z_range;
        const float B = 2.0 * far_z * near_z / z_range;

        util::Mat4x4f perspective{
            // row 0
            d / ar, // here we adjust based on aspect ratio.
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
            A,
            B,
            // row 3
            0.0f,
            0.0f,
            1.0f,
            0.0f,
        };

        FrameContext ctxt{ angle, delta, perspective, camera_transformation, translation, rot, WVP };

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
