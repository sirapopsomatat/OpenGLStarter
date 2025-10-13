#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"

#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList;

glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 0.0f);

Mesh *light;
static const char *lightVShader = "Shaders/lightShader.vert";
static const char *lightFShader = "Shaders/lightShader.frag";

static const char *bgVShader = "Shaders/bgShader.vert";
static const char *bgFShader = "Shaders/bgShader.frag";

Shader *depthShader;
static const char *depthVShader = "Shaders/depthShader.vert";
static const char *depthFShader = "Shaders/depthShader.frag";

float yaw = -90.0f;
float pitch = 0.0f;

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char *fShader = "Shaders/shader.frag";

GLuint uniformModel = 0;
GLuint uniformView = 0;
GLuint uniformProjection = 0;

unsigned int texture1;

Mesh *bg;

void CreateTriangle()
{
    GLfloat vertices[] =
        {
            // x    ,y  ,z          u,     ,v(Texture coordinates)
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.5f, 1.0f};

    unsigned int indices[] =
        {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2};

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    // meshList.push_back(obj1);
    for (int i = 0; i < 10; i++)
    {
        meshList.push_back(obj1);
    }
}

void CreateOBJ()
{
    Mesh *obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/suzanne.obj");
    if (loaded)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList.push_back(obj1);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }

    light = new Mesh();
    loaded = light->CreateMeshFromOBJ("Models/cube.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
        delete (light);
    }

    bg = new Mesh();
    loaded = bg->CreateMeshFromOBJ("Models/cube.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
        delete (bg);
    }
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->CreateFromFiles(lightVShader, lightFShader);
    shaderList.push_back(shader2);

    Shader *shader3 = new Shader();
    shader3->CreateFromFiles(bgVShader, bgFShader);
    shaderList.push_back(shader3);

    depthShader = new Shader();
    depthShader->CreateFromFiles(depthVShader, depthFShader);
}

void checkMouse()
{
    double mouseX, mouseY;
    glfwGetCursorPos(mainWindow.getWindow(), &mouseX, &mouseY);

    static float lastX = mouseX;
    static float lastY = mouseY;

    float xOffset = mouseX - lastX;
    float yOffset = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void RenderScene(glm::mat4 view, glm::mat4 projection)
{
    glm::vec3 pyramidPositions[] =
        {
            glm::vec3(0.0f, 0.0f, -2.5f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)};
    for (int i = 0; i < 10; i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pyramidPositions[i]);
        model = glm::rotate(model, glm::radians(2.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        // light
        //  glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat*)&lightColour);
        //  glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat*)&lightPos);
        //  glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat*)&cameraPos);

        // texture
        GLint uniformTexture1 = shaderList[0]->GetUniformLocation("texture1");
        glUniform1i(uniformTexture1, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        meshList[i]->RenderMesh();
    }
}

int main()
{
    std::cout << "[APP] Starting..." << std::endl;
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    std::cout << "[APP] Calling Window::initialise()" << std::endl;
    if (mainWindow.initialise() != 0)
    {
        std::cerr << "[APP] Failed to initialise window. See messages above for details." << std::endl;
        return -1;
    }
    std::cout << "[APP] Window initialised successfully." << std::endl;

    std::cout << "[APP] Loading model(s)..." << std::endl;
    CreateOBJ();
    std::cout << "[APP] Models loaded: " << meshList.size() << std::endl;
    std::cout << "[APP] Creating shaders..." << std::endl;
    CreateShaders();
    std::cout << "[APP] Shaders created: " << shaderList.size() << std::endl;

    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::cross(cameraRight, cameraDirection);

    // GLuint uniformModel = 0;
    // GLuint uniformView = 0;
    // GLuint uniformProjection = 0;

    // glm::mat4 projection(1.0f);
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);

    // textures
    // unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width1, height1, nrChannels1;
    unsigned char *data1 = stbi_load("Textures/uvmap.png", &width1, &height1, &nrChannels1, 0);

    if (!data1)
    {
        std::cerr << "[APP] Failed to load texture: " << stbi_failure_reason() << "\n";
    }
    else
    {
        // Handle 1/3/4 channel images correctly
        GLenum format = GL_RGB;
        if (nrChannels1 == 1)
            format = GL_RED;
        else if (nrChannels1 == 3)
            format = GL_RGB;
        else if (nrChannels1 == 4)
            format = GL_RGBA;

        // Avoid row-alignment issues for non-4-byte widths
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width1, height1, 0, format, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data1);
        std::cout << "[APP] Texture loaded (uvmap.png) size=" << width1 << "x" << height1 << " channels=" << nrChannels1 << std::endl;
    }
    glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(-10.0f, 0.0f, 10.0f);
    // glm::vec3 lightColour = glm::vec3(0.0f, 1.0f, 1.0f);

    // shadow map
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        // Get + Handle user input events
        glfwPollEvents();
        checkMouse();

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection = glm::normalize(direction);

        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraDirection * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= cameraDirection * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= cameraRight * 5.0f * deltaTime;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += cameraRight * 5.0f * deltaTime;
        }

        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw here
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformView = shaderList[0]->GetUniformLocation("view");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");

        glm::mat4 view(1.0f);

        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        // Object
        glm::vec3 pyramidPositions[] =
            {
                glm::vec3(0.0f, 0.0f, -2.5f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)};

        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        lightPos.z = 0.0f;

        // light
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

        // First pass - Depth Buffer
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, -8.0f), up);
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 20.0f);
        // glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, 0.1f, 20.0f);

        depthShader->UseShader();
        uniformModel = depthShader->GetUniformLocation("model");
        uniformView = depthShader->GetUniformLocation("view");
        uniformProjection = depthShader->GetUniformLocation("projection");

        RenderScene(lightView, lightProjection);

        // Second pass - scene
        glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformView = shaderList[0]->GetUniformLocation("view");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");

        RenderScene(view, projection);

        // light mesh render
        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformView = shaderList[1]->GetUniformLocation("view");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");
        glm::mat4 model(1.0f);

        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        // light
        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        light->RenderMesh();

        // bg
        shaderList[2]->UseShader();
        uniformModel = shaderList[2]->GetUniformLocation("model");
        uniformView = shaderList[2]->GetUniformLocation("view");
        uniformProjection = shaderList[2]->GetUniformLocation("projection");

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -8.0f));
        model = glm::scale(model, glm::vec3(20.0f, 20.0f, 0.1f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

        glm::vec3 bgColour = glm::vec3(0.22f, 1.0f, 0.08f);
        glUniform3fv(shaderList[2]->GetUniformLocation("bgColour"), 1, (GLfloat *)&bgColour);

        // Bind shadow map to texture unit 0 for the background shader and set sampler uniform
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint uniShadow = shaderList[2]->GetUniformLocation("shadowMap");
        if (uniShadow != -1)
        {
            glUniform1i(uniShadow, 0);
        }
        bg->RenderMesh();

        glUseProgram(0);
        // end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
