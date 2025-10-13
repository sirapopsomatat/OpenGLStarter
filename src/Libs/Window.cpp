#include "Window.h"

Window::Window()
{
    width = 800;
    height = 600;
    glfwMajorVersion = 3;
    glfwMinorVersion = 1;
}

Window::Window(GLint windowWidth, GLint windowHeight, GLint majorVersion, GLint minorVersion)
{
    width = windowWidth;
    height = windowHeight;
    glfwMajorVersion = majorVersion;
    glfwMinorVersion = minorVersion;
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}

int Window::initialise()
{
    // Register an error callback so we get useful messages on failure
    static auto glfw_error_callback = [](int error, const char *description)
    {
        fprintf(stderr, "GLFW Error (%d): %s\n", error, description ? description : "(null)");
    };
    glfwSetErrorCallback(glfw_error_callback);

    // init GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialisation failed!\n");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version (using 3.1)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwMinorVersion);

    // Core Profile = No Backwards Compatibility
    //  Use the standard profile hint (don't use a non-standard ANY_PROFILE hint)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);

    if (!mainWindow)
    {
        fprintf(stderr, "GLFW window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);
    // Ensure the window is shown (some platforms or flags may hide it)
    glfwShowWindow(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "GLEW initialisation failed!\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    return 0;
}