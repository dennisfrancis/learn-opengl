#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <stb_image.h>

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

    // clang-format off
    float vertices[] = {
        // counter clock wise.

        // top left
        // position
        -0.5f, 0.5f, 0.0f,
        // color
        1.0f, 1.0f, 0.0f,
        // texture coords
        0.0f, 1.0f,

        // bottom left
        // position
        -0.5f, -0.5f, 0.0f,
        // color
        0.0f, 0.0f, 1.0f,
        // texture coords
        0.0f, 0.0f,

        // bottom right
        // position
        0.5f, -0.5f, 0.0f,
        // color
        0.0f, 1.0f, 0.0f,
        // texture coords
        1.0f, 0.0f,

        // top right
        // position
        0.5f, 0.5f, 0.0f,
        // color
        1.0f, 0.0f, 0.0f,
        // texture coords
        1.0f, 1.0f,

    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3, // second triangle
    };

    // clang-format on

    GLuint vao;
    glGenVertexArrays(1, &vao);
    // Bind vertex array object.
    glBindVertexArray(vao);
    // Bind the vbo and tell VAO its memory layouts.
    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0 /* offset */);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Attributes are disabled by default. We need to explicitly enable them.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // load and create a texture.
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT is the default.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture, generate mipmaps.
    int tex_width, tex_height, num_channels;
    unsigned char* data
        = stbi_load("resources/textures/container.jpg", &tex_width, &tex_height, &num_channels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "[ERROR] Failed to load the texture!\n";
    }
    stbi_image_free(data);

    // Optional: Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Little optimization to skip the other side of the triangle.
    // We are drawing the triangle in counter clockwise dir.
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CW for clock-wise

    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here...
        // set clear color (state setter)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        // bind texture.
        glBindTexture(GL_TEXTURE_2D, texture);

        shader_program.use();
        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // No need to unbind it every time.
        // glBindVertexArray(0);

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);

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
