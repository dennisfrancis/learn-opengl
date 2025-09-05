#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void CreateVertexBuffer(GLuint& VBO);

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

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    GLuint VBO;
    CreateVertexBuffer(VBO);
    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here...
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // Clear the color buffer using the clear-color state set above.
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(0);
        // Tell OpenGL how it should interpret the vertex data in the buffer.
        glVertexAttribPointer(
            // Specifies the index of the generic vertex attribute to be modified.
            0 /* here, only one attribute which is the positions */,
            // Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4
            3 /* number of elements in a single vertex */,
            // Specifies the data type of each component in the array.
            GL_FLOAT /* type of single element (here vertex position coordinate) */,
            // specifies whether fixed-point data values should be normalized.
            GL_FALSE,
            // Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.
            0,
            // Specifies a offset of the first component of this generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target.
            0);

        glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(0);

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

void CreateVertexBuffer(GLuint& VBO)
{
    float vertices[3] = { // only one vertex.
                          0.0f, 0.0f, 0.0f
    };

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW /* Store once, ready many times */);
}
