#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

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

    // We have to tell OpenGL the size of the rendering window so OpenGL knows
    // how we want to display the data and coordinates with respect to the
    // window.
    // We could actually set the viewport dimensions at values smaller than
    // GLFW’s dimensions; then all the OpenGL rendering would be displayed in a
    // smaller window
    //
    // Behind the scenes OpenGL uses the data specified via glViewport to
    // transform the 2D coordinates it processed to coordinates on your screen.
    // For example, a processed point of location (-0.5,0.5) would (as its
    // final transformation) be mapped to (200,450) in screen coordinates. Note
    // that processed coordinates in OpenGL are between -1 and 1 so we
    // effectively map from the range (-1 to 1) to (0, 800) and (0, 600)
    glViewport(0, 0, width, height);

    // However, the moment a user resizes the window the viewport should be
    // adjusted as well. We can register a callback function on the window that
    // gets called each time the window is resized.
    // So register a callback that adjusts the gl viewport to full available
    // size.
    //
    // We register the callback functions after we’ve created the window and
    // before the render loop is initiated.
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    // Simple render loop.
    while (!glfwWindowShouldClose(window))
    {
        // Double buffer When an application draws in a single buffer the
        // resulting image may display flickering issues. This is because the
        // resulting output image is not drawn in an instant, but drawn pixel
        // by pixel and usually from left to right and top to bottom. Because
        // this image is not displayed at an instant to the user while still
        // being rendered to, the result may contain artifacts. To circumvent
        // these issues, windowing applications apply a double buffer for
        // rendering. The front buffer contains the final output image that is
        // shown at the screen, while all the rendering commands draw to the
        // back buffer. As soon as all the rendering commands are finished we
        // swap the back buffer to the front buffer so the image can be
        // displayed without still being rendered to, removing all the
        // aforementioned artifacts.
        glfwSwapBuffers(window);

        // Checks if any events are triggered (like keyboard input or mouse
        // movement events), updates the window state, and calls the
        // corresponding functions (which we can register via callback
        // methods).
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
