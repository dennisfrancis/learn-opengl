#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <iostream>

#include <util/shader.hpp>

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window, int& key_pressed);
static void calc_mix(int key_pressed, float& mix_amt, float mix_step);

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
    unsigned int texture[2];
    glGenTextures(2, texture);
    const char* images[2]
        = { "resources/textures/container.jpg", "resources/textures/awesomeface.png" };

    // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side
    // of the image, but images usually have 0.0 at the top of the y-axis.
    stbi_set_flip_vertically_on_load(
        true); // tell stb_image.h to flip loaded textures on the y-axis.
    for (int i = 0; i < 2; ++i)
    {
        // activate the texture unit first before binding texture.
        // Texture units are needed for using multiple textures at once.
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        // set the texture wrapping parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT is the default.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture, generate mipmaps.
        int tex_width, tex_height, num_channels;
        unsigned char* data = stbi_load(images[i], &tex_width, &tex_height, &num_channels, 0);
        if (data)
        {
            // note that the second image has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA.
            unsigned int image_pixel_fmt = (i == 0 ? GL_RGB : GL_RGBA);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, image_pixel_fmt,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "[ERROR] Failed to load the texture " << images[i] << '\n';
        }
        stbi_image_free(data);
    }

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

    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once).
    shader_program.use(); // don't forget to activate/use the shader before setting uniforms!
    shader_program.set_int("texture0", 0);
    shader_program.set_int("texture1", 1);

    float mix_amt = 0.2;
    constexpr float mix_step = 0.005;
    int key_pressed = 0;

    while (!glfwWindowShouldClose(window))
    {
        // input
        key_pressed = 0;
        process_input(window, key_pressed);
        calc_mix(key_pressed, mix_amt, mix_step);

        // rendering commands here...
        // set clear color (state setter)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units.
        for (int i = 0; i < 2; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture[i]);
        }

        shader_program.use();
        shader_program.set_float("mix_amt", mix_amt);
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
    glDeleteTextures(2, texture);

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// We call this in the main loop.
void process_input(GLFWwindow* window, int& key_pressed)
{
    // Returns the last reported state of a keyboard key for the specified
    // window.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    constexpr int keys[] = { GLFW_KEY_UP, GLFW_KEY_DOWN };
    for (const int& kk : keys)
    {
        if (glfwGetKey(window, kk) == GLFW_PRESS)
        {
            key_pressed = kk;
            break;
        }
    }
}

static void calc_mix(int key_pressed, float& mix_amt, float mix_step)
{
    if (key_pressed == GLFW_KEY_UP)
    {
        mix_amt = std::min(1.0f, mix_amt + mix_step);
    }
    else if (key_pressed == GLFW_KEY_DOWN)
    {
        mix_amt = std::max(0.0f, mix_amt - mix_step);
    }
}
