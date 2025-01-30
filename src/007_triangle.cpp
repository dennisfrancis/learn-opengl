#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

// vertex shader source code in GLSL.
static const char* vertex_shader_source
    = "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      // gl_Position is where the output of the vertex shader is placed which
      // is a vec4.
      " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

static const char* fragment_shader_source = "#version 330 core\n"
                                            "out vec4 FragColor;\n"
                                            "void main()\n"
                                            "{\n"
                                            "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                            "}\0";

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);

static bool add_compile_shader(unsigned int shader_object_id, const char** shader_source_string,
                               GLenum shader_type);

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

    // To store id of vertex array object(VAO).
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    // clang-format on

    // Bind vertex array object.
    glBindVertexArray(VAO);

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
    if (!add_compile_shader(vertex_shader, &vertex_shader_source, GL_VERTEX_SHADER))
    {
        // Compile failed. add_compile_shader will output the error.
        glfwTerminate();
        return 1;
    }

    // The fragment shader is all about calculating the color output of your
    // pixels. In this case it sets the same color to every pixel.
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // Next we attach the shader source code to the shader object and compile
    // the shader:
    if (!add_compile_shader(fragment_shader, &fragment_shader_source, GL_FRAGMENT_SHADER))
    {
        // Compile failed. add_compile_shader will output the error.
        glfwTerminate();
        return 1;
    }
    // Both the shaders are now compiled and the only thing left to do is link
    // both shader objects into a *shader program* that we can use for
    // rendering.
    // A shader program object is the final linked version of multiple shaders
    // combined.

    unsigned int shader_program;
    shader_program = glCreateProgram();
    // Now we need to attach the previously compiled shaders to the program
    // object and then link them.
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info_log[512];
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        std::cerr << "[ERROR] Program link failed!\n" << info_log << '\n';
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glfwTerminate();
        return 1;
    }
    // The result is a program object that we can activate by calling
    // glUseProgram.

    // Tell OpenGL how it should interpret the vertex data in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Enable the vertex attribute.
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards
    // we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other VAOs requires
    // a call to glBindVertexArray anyways so we generally don't unbind VAOs
    // (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here...
        // set clear color (state setter)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        // glUseProgram with the newly created program object as its argument.
        glUseProgram(shader_program);
        // Usually when you have multiple objects you want to draw, you first
        // generate/configure all the VAOs (and thus the required VBO and
        // attribute pointers) and store those for later use. The moment we
        // want to draw one of our objects, we take the corresponding VAO, bind
        // it, then draw the object and unbind the VAO again.
        // Seeing as we only have a single VAO there's no need to bind it every
        // time, but we'll do so to keep things a bit more organized
        glBindVertexArray(VAO);

        // Draw the triangle using the active shader program.
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // No need to unbind it every time.
        // glBindVertexArray(0);

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

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

bool add_compile_shader(unsigned int shader_object_id, const char** shader_source_string,
                        GLenum shader_type)
{
    // Next we attach the shader source code to the shader object and compile
    // the shader:
    // second param is the number of strings passed.
    glShaderSource(shader_object_id, 1, shader_source_string, NULL);
    glCompileShader(shader_object_id);
    // Check for compile errors:
    int success;
    glGetShaderiv(shader_object_id, GL_COMPILE_STATUS, &success);
    if (success)
    {
        return true;
    }
    char info_log[512];
    // Compilation failed. Show the errors and bail out.
    glGetShaderInfoLog(shader_object_id, sizeof(info_log), NULL, info_log);
    std::cerr << "[ERROR] " << (shader_type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
              << " Shader compile failed:\n"
              << info_log << '\n';
    return false;
}
