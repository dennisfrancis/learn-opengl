#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

// vertex shader source code in GLSL.
const char* vertex_shader_source
    = "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      // gl_Position is where the output of the vertex shader is placed which
      // is a vec4.
      " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

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

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    // clang-format on

    // ID of vertex buffer object.
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // OpenGL has many types of buffer objects and the buffer type of a vertex
    // buffer object is GL_ARRAY_BUFFER. OpenGL allows us to bind to several
    // buffers at once as long as they have a different buffer type.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // From this point on any buffer calls we make (on the GL_ARRAY_BUFFER
    // target) will be used to configure the currently bound buffer, which is
    // VBO.

    // Now copy the vertices data defined earlier to this currently bound
    // buffer object.
    // Second param is size of data in bytes.
    // Fourth param specifies how we want the graphics card to manage the given
    // data.
    // * GL_STREAM_DRAW: the data is set only once and used by the GPU at most
    //                   a few times.
    // * GL_STATIC_DRAW: the data is set only once and used many times.
    // * GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Now the data is in the GPU memory and now we can write vertex and
    // fragment shaders to process this data.

    // In order for OpenGL to use the shader it has to dynamically compile it
    // at run-time from its source code. The first thing we need to do is
    // create a shader object, again referenced by an ID.
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Next we attach the shader source code to the shader object and compile
    // the shader:
    // second param is the number of strings passed.
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    // Check for compile errors:
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // Compilation failed. Show the errors and bail out.
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
        std::cerr << "[ERROR] Vertex shader compile failed!\n" << info_log << '\n';
        glfwTerminate();
        return 1;
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

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
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
