#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <util/shader.hpp>

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);

struct Point
{
    float x;
    float y;
};
const float root3 = std::sqrt(3.0f);

static Point next_center(float minx, float maxx, float miny);
static void draw_triangles(float scale, const Point& center, const unsigned int vao,
                           util::Shader& shader_program);

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

    glm::vec4 bgcolor{ 0.2f, 0.3f, 0.3f, 1.0f };

    const Point center{ next_center(-1.0f + 0.0f, -1.0f + 2.0f, -1.0f + 0.0f) };

    const Point triangle[3]{
        // top-left
        { -0.5f, 0.5f / root3 },
        // bottom-center
        { 0.0f, -1.0f / root3 },
        // top-right
        { 0.5f, 0.5f / root3 },
    };

    // clang-format off
    float vertices[] = {
        // counter clock wise.

        // top left
        // position
        triangle[0].x, triangle[0].y, 0.0f,
        // texture coords
        -0.5f, 1.05f,

        // bottom center
        // position
        triangle[1].x, triangle[1].y, 0.0f,
        // texture coords
        0.5f, -0.45f,

        // top right
        // position
        triangle[2].x, triangle[2].y, 0.0f,
        // texture coords
        1.5f, 1.05f,

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0 /* offset */);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Attributes are disabled by default. We need to explicitly enable them.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // load and create a texture.
    unsigned int texture;
    glGenTextures(1, &texture);
    const char* images[2]
        = { "resources/textures/container.jpg", "resources/textures/awesomeface.png" };

    // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side
    // of the image, but images usually have 0.0 at the top of the y-axis.
    stbi_set_flip_vertically_on_load(
        true); // tell stb_image.h to flip loaded textures on the y-axis.
    {
        // activate the texture unit first before binding texture.
        // Texture units are needed for using multiple textures at once.
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_BORDER); // GL_REPEAT is the default.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(bgcolor));
        // set texture filtering parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture, generate mipmaps.
        int tex_width, tex_height, num_channels;
        unsigned char* data = stbi_load(images[1], &tex_width, &tex_height, &num_channels, 0);
        if (data)
        {
            // note that the second image has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "[ERROR] Failed to load the texture " << images[1] << '\n';
        }
        stbi_image_free(data);
    }

    // Optional: Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Little optimization to skip the other side of the triangle.
    // We are drawing the triangle in counter clockwise dir.
    // glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK); // cull back face
    // glFrontFace(GL_CCW); // GL_CW for clock-wise

    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once).
    shader_program.use(); // don't forget to activate/use the shader before setting uniforms!
    shader_program.set_int("texture0", 0);
    shader_program.set_vec4("bgcolor", bgcolor);

    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here...
        // set clear color (state setter)
        glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units.
        {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }

        draw_triangles(1.0f, center, vao, shader_program);

        // swap buffers
        glfwSwapBuffers(window);
        // poll and process events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture);

    glfwTerminate();
    return 0;
}

Point next_center(float minx, float maxx, float miny)
{
    float scale = maxx - minx;
    return Point{ minx + 0.5f * scale, miny + 0.5f * scale / root3 };
}

void draw_triangles(float scale, const Point& center, const unsigned int vao,
                    util::Shader& shader_program)
{
    shader_program.use();
    // create transformations.
    glm::mat4 trans{ 1.0f }; // make sure to initialize matrix to identity matrix first.
    trans = glm::translate(trans, glm::vec3(center.x, center.y, 0.0f));
    trans = glm::scale(trans, glm::vec3(scale, scale, 1.0f));
    shader_program.set_matrix4f("transform", trans);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (scale <= 1.0f / 16.0)
    {
        return;
    }

    // Upper
    draw_triangles(scale * 0.5, Point{ center.x, center.y + scale / root3 }, vao, shader_program);
    // Left
    draw_triangles(scale * 0.5, Point{ center.x - scale * 0.5f, center.y - scale / (2.0f * root3) },
                   vao, shader_program);
    // Right
    draw_triangles(scale * 0.5, Point{ center.x + scale * 0.5f, center.y - scale / (2.0f * root3) },
                   vao, shader_program);
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
