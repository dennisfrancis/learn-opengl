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

static const char* fragment_shader_source[2] = { "#version 330 core\n"
                                                 "out vec4 frag_color;\n"
                                                 "void main()\n"
                                                 "{\n"
                                                 "  frag_color = vec4(0.5f, 0.0f, 0.5f, 1.0f);\n"
                                                 "}\0",

                                                 // Second fragment shader with different color.
                                                 "#version 330 core\n"
                                                 "out vec4 frag_color;\n"
                                                 "void main()\n"
                                                 "{\n"
                                                 "  frag_color = vec4(0.5f, 0.5f, 0.0f, 1.0f);\n"
                                                 "}\0" };

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

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices1[] = {
        // Counter-clock wise.
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    float vertices2[] = {
         // Second triangle.
         0.5f,   -0.75f,  0.0f,
         0.75f,  -0.75f,  0.0f,
         0.625f, -0.625f, 0.0f,
    };

    // clang-format on

    // Create two VAOs and VBOs.
    GLuint vaos[2]{};
    GLuint vbos[2]{};
    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    // Setup first VAO and VBO.
    glBindVertexArray(vaos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // Both vertex buffers have the same layout.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Attributes are disabled by default. We need to explicitly enable them too.
    glEnableVertexAttribArray(0);

    // Setup second VAO and VBO.
    glBindVertexArray(vaos[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    // Both vertex buffers have the same layout.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Attributes are disabled by default. We need to explicitly enable them too.
    glEnableVertexAttribArray(0);

    // Optional: Unbind VAO and VBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Setup shaders and program.
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    if (!add_compile_shader(vertex_shader, &vertex_shader_source, GL_VERTEX_SHADER))
    {
        // Compile failed. add_compile_shader will output the error.
        glfwTerminate();
        return 1;
    }

    unsigned int fragment_shaders[2];
    for (int i = 0; i < 2; ++i)
    {
        fragment_shaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
        if (!add_compile_shader(fragment_shaders[i], &fragment_shader_source[i],
                                GL_FRAGMENT_SHADER))
        {
            // Compile failed. add_compile_shader will output the error.
            glfwTerminate();
            return 1;
        }
    }

    unsigned int shader_programs[2];

    for (int i = 0; i < 2; ++i)
    {
        shader_programs[i] = glCreateProgram();
        // Now we need to attach the previously compiled shaders to the program
        // object and then link them.
        glAttachShader(shader_programs[i], vertex_shader);
        glAttachShader(shader_programs[i], fragment_shaders[i]);
        glLinkProgram(shader_programs[i]);
        int success;
        glGetProgramiv(shader_programs[i], GL_LINK_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetProgramInfoLog(shader_programs[i], sizeof(info_log), NULL, info_log);
            std::cerr << "[ERROR] Program link failed!\n" << info_log << '\n';
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shaders[0]);
            glDeleteShader(fragment_shaders[1]);
            glfwTerminate();
            return 1;
        }
    }

    // Can delete the shader now that we have made a shader-program.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shaders[0]);
    glDeleteShader(fragment_shaders[1]);

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

        glUseProgram(shader_programs[0]);
        // Draw first triangle using first VAO.
        glBindVertexArray(vaos[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Use second program to draw in different color.
        glUseProgram(shader_programs[1]);
        // Draw second triangle using second VAO.
        glBindVertexArray(vaos[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // No need to unbind it every time.
        // glBindVertexArray(0);

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(2, vaos);
    glDeleteBuffers(2, vbos);
    glDeleteProgram(shader_programs[0]);
    glDeleteProgram(shader_programs[1]);

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
