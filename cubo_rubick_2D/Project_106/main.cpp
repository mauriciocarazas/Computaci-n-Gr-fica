#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <vector>

#include "vertex.h"
#include "anillo.h"
#include "esferas.h"
#include "cuboEsferas.h"
#include "camara.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int VBO, VAO, EBO;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = 
"#version 330 core\n"
"out vec4 fragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   fragColor = vec4(ourColor, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void updateVertexBuffer(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// Camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Time tracking
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    CuboEsferas cuboEsferasR(Vertex(0.25f, -0.20f, 0.0f), 1.0f, 0.0f, 0.0f, 90.0f); // Esferas rojas
    CuboEsferas cuboEsferasB(Vertex(0.0f, 0.15f, 0.0f), 0.0f, 0.0f, 1.0f, 0.0f); // Esferas azules
    CuboEsferas cuboEsferasG(Vertex(-0.25f, -0.20f, 0.0f), 0.0f, 1.0f, 0.0f, -90.0f); // Esferas verdes

    CuboEsferas cuboEsferasAmarillo(Vertex(0.55f, 0.2f, 0.0f), 1.0f, 1.0f, 0.0f, 90.0f); // Esferas amarillas
    CuboEsferas cuboEsferasMagenta(Vertex(-0.55f, 0.2f, 0.0f), 1.0f, 0.0f, 1.0f, -90.0f); // Esferas magenta
    CuboEsferas cuboEsferasCian(Vertex(0.0f, -0.75f, 0.0f), 0.0f, 1.0f, 1.0f, 180.0f); // Esferas cian


    Anillo anillo1(0.0f, 0.3f, 0.45f, 1.0f, 1.0f, 1.0f);//anillo superior pequeno
    Anillo anillo2(0.0f, 0.3f, 0.55f, 1.0f, 1.0f, 1.0f);//anillo superior mediano
    Anillo anillo3(0.0f, 0.3f, 0.65f, 1.0f, 1.0f, 1.0f);//anillo superior grande

    Anillo anillo4(-0.3f, -0.3f, 0.45f, 1.0f, 1.0f, 1.0f);
    Anillo anillo5(-0.3f, -0.3f, 0.55f, 1.0f, 1.0f, 1.0f);
    Anillo anillo6(-0.3f, -0.3f, 0.65f, 1.0f, 1.0f, 1.0f);

    Anillo anillo7(0.3f, -0.3f, 0.45f, 1.0f, 1.0f, 1.0f);
    Anillo anillo8(0.3f, -0.3f, 0.55f, 1.0f, 1.0f, 1.0f);
    Anillo anillo9(0.3f, -0.3f, 0.65f, 1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Create transformations
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Pass transformation matrices to the shader
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // ESFERAS
        for (const auto& esfera : cuboEsferasR.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasB.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasG.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasAmarillo.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasMagenta.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasCian.esferas) {
            updateVertexBuffer(esfera.vertices, esfera.indices);
            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }

        updateVertexBuffer(anillo1.vertices, anillo1.indices);
        glDrawElements(GL_LINES, anillo1.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo2.vertices, anillo2.indices);
        glDrawElements(GL_LINES, anillo2.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo3.vertices, anillo3.indices);
        glDrawElements(GL_LINES, anillo3.indices.size(), GL_UNSIGNED_INT, 0);

        updateVertexBuffer(anillo4.vertices, anillo4.indices);
        glDrawElements(GL_LINES, anillo4.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo5.vertices, anillo5.indices);
        glDrawElements(GL_LINES, anillo5.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo6.vertices, anillo6.indices);
        glDrawElements(GL_LINES, anillo6.indices.size(), GL_UNSIGNED_INT, 0);

        updateVertexBuffer(anillo7.vertices, anillo7.indices);
        glDrawElements(GL_LINES, anillo7.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo8.vertices, anillo8.indices);
        glDrawElements(GL_LINES, anillo8.indices.size(), GL_UNSIGNED_INT, 0);
        updateVertexBuffer(anillo9.vertices, anillo9.indices);
        glDrawElements(GL_LINES, anillo9.indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
