#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <vector>

unsigned int VBO, VAO, EBO;

const int longitudeSegments = 10;
const int latitudeSegments = 10;
const int numVertices = (longitudeSegments + 1) * (latitudeSegments + 1);
const int numIndices = longitudeSegments * latitudeSegments * 6;

struct Vertex {
    float x, y, z;
    float r, g, b;
    Vertex() : x(0), y(0), z(0), r(0), g(0), b(0) {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z), r(0), g(0), b(0) {}
    Vertex(float x, float y, float z, float r, float g, float b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
};

struct Esfera {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float colorR, colorG, colorB;
    float centerX, centerY, centerZ;

    Esfera(float radius, float r, float g, float b, float x, float y, float z) : colorR(r), colorG(g), colorB(b), centerX(x), centerY(y), centerZ(z) {
        generateSphere(radius);
    }

    void generateSphere(float radius) {
        int vertexIndex = 0;
        int index = 0;

        vertices.resize(numVertices);
        indices.resize(numIndices);

        for (int i = 0; i <= latitudeSegments; ++i) {
            float theta = i * M_PI / latitudeSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int j = 0; j <= longitudeSegments; ++j) {
                float phi = j * 2 * M_PI / longitudeSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;

                vertices[vertexIndex++] = Vertex(radius * x + centerX, radius * y + centerY, radius * z + centerZ, colorR, colorG, colorB);
            }
        }

        for (int i = 0; i < latitudeSegments; ++i) {
            for (int j = 0; j < longitudeSegments; ++j) {
                int first = (i * (longitudeSegments + 1)) + j;
                int second = first + longitudeSegments + 1;

                indices[index++] = first;
                indices[index++] = second;
                indices[index++] = first + 1;

                indices[index++] = second;
                indices[index++] = second + 1;
                indices[index++] = first + 1;
            }
        }
    }
};

struct CuboEsferas {
    std::vector<Esfera> esferas;
    Vertex center;
    int numEsferas = 9;

    CuboEsferas(Vertex c, float r, float g, float b) : center(c) {
        generateCuboEsferas(r, g, b);
    }

    void generateCuboEsferas(float r, float g, float b) {
        float radius = 0.025f;
        float spacing = 0.09f;
        float startX = center.x - spacing;
        float startY = center.y - spacing;
        float startZ = center.z;

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                float x = startX + j * spacing;
                float y = startY + i * spacing;
                float z = startZ;
                esferas.push_back(Esfera(radius, r, g, b, x, y, z));
            }
        }
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec3 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(position, 1.0);\n"
"   vertexColor = color;\n"
"}\0";

const char *fragmentShaderSource = 
"#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"   fragColor = vec4(vertexColor, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void updateVertexBuffer(const Esfera& esfera) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, esfera.vertices.size() * sizeof(Vertex), esfera.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, esfera.indices.size() * sizeof(unsigned int), esfera.indices.data(), GL_STATIC_DRAW);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float cameraSpeed = 0.1f; // Velocidad de movimiento de la cámara

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

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

#ifdef _APPLE_
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
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    CuboEsferas cuboEsferasR(Vertex(0.3f, -0.15f, 0.0f), 1.0f, 0.0f, 0.0f); // Esferas rojas
    CuboEsferas cuboEsferasB(Vertex(0.0f, 0.1f, 0.0f), 0.0f, 0.0f, 1.0f); // Esferas azules
    CuboEsferas cuboEsferasG(Vertex(-0.3f, -0.15f, 0.0f), 0.0f, 1.0f, 0.0f); // Esferas verdes

    CuboEsferas cuboEsferasAmarillo(Vertex(0.35f, 0.35f, 0.0f), 1.0f, 1.0f, 0.0f); // Esferas amarillas
    CuboEsferas cuboEsferasMagenta(Vertex(-0.35f, 0.35f, 0.0f), 1.0f, 0.0f, 1.0f); // Esferas magenta
    CuboEsferas cuboEsferasCian(Vertex(0.0f, -0.5f, 0.0f), 0.0f, 1.0f, 1.0f); // Esferas cian


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        std::cout<<"RUN ..."<< std::endl;
        glfwSetKeyCallback(window, key_callback);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        for (const auto& esfera : cuboEsferasR.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasB.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasG.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasAmarillo.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasMagenta.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (const auto& esfera : cuboEsferasCian.esferas) {
            std::cout << "Esfera en: " << esfera.centerX << ", " << esfera.centerY << ", " << esfera.centerZ << std::endl;
            std::cout << "Color: " << esfera.colorR << ", " << esfera.colorG << ", " << esfera.colorB << std::endl;

            updateVertexBuffer(esfera);

            glDrawElements(GL_TRIANGLES, esfera.indices.size(), GL_UNSIGNED_INT, 0);
        }


        glBindVertexArray(0);
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
