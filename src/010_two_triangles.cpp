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
                                            "out vec4 frag_color;\n"
                                            "void main()\n"
                                            "{\n"
                                            "  frag_color = vec4(0.5f, 0.0f, 0.5f, 1.0f);\n"
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

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices[] = {
        // Counter-clock wise.
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
         // Second triangle.
         0.5f,    -0.75f,  0.0f,
         0.75f,  -0.75f,  0.0f,
         0.625f, -0.625f, 0.0f,
    };

    // clang-format on

    // Create a VBO for positions.
    GLuint points_vbo{};
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    // Bind vertex array object.
    glBindVertexArray(vao);
    // Bind the two vbo's and tell VAO their memory layouts.
    // 1. Vertex positions:
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    // Attach it to the 0th attrib pointer.
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

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!add_compile_shader(fragment_shader, &fragment_shader_source, GL_FRAGMENT_SHADER))
    {
        // Compile failed. add_compile_shader will output the error.
        glfwTerminate();
        return 1;
    }

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

    // Can delete the shader now that we have made a shader-program.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

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

        glUseProgram(shader_program);
        glBindVertexArray(vao);

        // Draw the two triangles (6 elements) using the active shader program.
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // No need to unbind it every time.
        // glBindVertexArray(0);

        // poll and process events
        glfwPollEvents();
        // swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &points_vbo);
    glDeleteProgram(shader_program);

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
