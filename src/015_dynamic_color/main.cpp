#include <cmath>
#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window);

static bool add_compile_shader(unsigned int shader_object_id, const char** shader_source_string,
                               GLenum shader_type);
static bool load_shader(const std::string& fname, std::string& buffer);

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
    std::string vert_shader;
    load_shader("shaders/vertex.vert", vert_shader);
    const char* vertex_shader_source = vert_shader.c_str();
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    if (!add_compile_shader(vertex_shader, &vertex_shader_source, GL_VERTEX_SHADER))
    {
        // Compile failed. add_compile_shader will output the error.
        glfwTerminate();
        return 1;
    }

    std::string frag_shader;
    load_shader("shaders/fragment.frag", frag_shader);
    const char* fragment_shader_source = frag_shader.c_str();
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

    // Vertices of the triangle we want to render is first specified in
    // NDCoordinates.
    // clang-format off
    float vertices[] = {
        // Counter-clock wise.
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
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

        // update uniform variable with time varying color.
        double time_value = glfwGetTime();
        // make sure value varies between [0.0, 1.0].
        float green_value = static_cast<float>(std::sin(time_value) / 2.0 + 0.5);
        // Query for the location of the my_color uniform.
        int color_location = glGetUniformLocation(shader_program, "my_color");
        if (color_location == -1)
        {
            std::cerr << "[ERROR] Unable to get location of uniform variable my_color\n";
            break;
        }
        // Set the uniform value using the glUniform4f function.
        glUniform4f(color_location, 0.0, green_value, 0.0, 1.0);

        glDrawArrays(GL_TRIANGLES, 0, 3);
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

bool load_shader(const std::string& fname, std::string& buffer)
{
    std::ifstream fin(fname);
    if (!fin)
    {
        std::cerr << "[ERROR] Error opening file " << fname << '\n';
        return false;
    }
    fin.seekg(0, std::ios::end);
    buffer.reserve(fin.tellg());
    fin.seekg(0, std::ios::beg);
    buffer.assign((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();
    return true;
}
