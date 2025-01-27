#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << '\n';
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;

    // Data in RAM to be copied to GPU memory.
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    // Create a buffer in GPU and get back its ID.
    // ID will be stored in an unsigned int variable.
    unsigned int buffer;
    glGenBuffers(1, &buffer);

    // Select that buffer in GPU. (it works like a state machine)
    // Specify that it will be used as an array.
    // ID need to be passed to select the buffer we want.
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // Copy data to the selected buffer.
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // This doesn't draw anything. See next file for that.

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
