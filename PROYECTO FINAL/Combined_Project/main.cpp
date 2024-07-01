#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <vector>

#include "Rubik.h"
#include "AllSolver/solve.cpp"
#include "AllSolver/random.cpp"
#include "AllSolver/search.cpp"
#include "vertex.h"
#include "anillo.h"
#include "esferas.h"
#include "cuboEsferas.h"
#include "camara.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int VBO, VAO, EBO;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void processInputRubik(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

float camera_pos_x = 0.0f, camera_pos_y = 0.0f, camera_pos_z = 0.0f;
glm::vec3 cameraPos = glm::vec3(camera_pos_x, camera_pos_y, camera_pos_z);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

float yaw = 0.0f;
float pitch = 0;
float cam_x = 0, cam_y = 0;
float cameraSpeed = 3.0; 
std::string condition_input = "N";
std::vector<std::string> reg_mov;
std::vector<std::string> solution_cubo;
Rubik c;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec3 aColor;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec3 ourColor;\n"
"struct Light {\n"
"    vec3 position;\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"};\n"
"struct Material {\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"    float shininess;\n"
"};\n"
"uniform Light light;\n"
"uniform Material material;\n"
"uniform vec3 viewPos;\n"
"void main()\n"
"{\n"
"    vec3 ambient = light.ambient * material.ambient;\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(light.position - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"    vec3 specular = light.specular * (spec * material.specular);\n"
"    vec3 result = ambient + diffuse + specular;\n"
"    FragColor = vec4(result * ourColor, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void updateVertexBuffer(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Combined Project", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

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

    glBindVertexArray(VAO);

    CuboEsferas cuboEsferasR(Vertex(0.25f, -0.20f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f), 90.0f);
    CuboEsferas cuboEsferasB(Vertex(0.0f, 0.15f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f), 0.0f);
    CuboEsferas cuboEsferasG(Vertex(-0.25f, -0.20f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f), -90.0f);
    CuboEsferas cuboEsferasAmarillo(Vertex(0.55f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f), 90.0f);
    CuboEsferas cuboEsferasMagenta(Vertex(-0.55f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f), -90.0f);
    CuboEsferas cuboEsferasCian(Vertex(0.0f, -0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f), 180.0f);

    Anillo anillo1(0.0f, 0.3f, 0.45f, 1.0f, 1.0f, 1.0f); // Anillo superior pequeño
    Anillo anillo2(0.0f, 0.3f, 0.55f, 1.0f, 1.0f, 1.0f); // Anillo superior mediano
    Anillo anillo3(0.0f, 0.3f, 0.65f, 1.0f, 1.0f, 1.0f); // Anillo superior grande

    Anillo anillo4(-0.3f, -0.3f, 0.45f, 1.0f, 1.0f, 1.0f);
    Anillo anillo5(-0.3f, -0.3f, 0.55f, 1.0f, 1.0f, 1.0f);
    Anillo anillo6(-0.3f, -0.3f, 0.65f, 1.0f, 1.0f, 1.0f);

    Anillo anillo7(0.3f, -0.3f, 0.45f, 1.0f, 1.0f, 1.0f);
    Anillo anillo8(0.3f, -0.3f, 0.55f, 1.0f, 1.0f, 1.0f);
    Anillo anillo9(0.3f, -0.3f, 0.65f, 1.0f, 1.0f, 1.0f);

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    unsigned char *data = stbi_load("C:/Users/mauri/Downloads/universidad/LabCompiler2023_II_CG/glfw-master/OwnProjects/cubo_rubick_13/imagen.png", &width, &height, &nrChannels, 0);
    if (data){   
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, texture);

    std::cout << "Girar en sentido horario" << std::endl;
    std::cout << "Con la tecla U la cara UP" << std::endl;
    std::cout << "Con la tecla D la cara DOWN" << std::endl;
    std::cout << "Con la tecla L la cara LEFT" << std::endl;
    std::cout << "Con la tecla R la cara RIGHT" << std::endl;
    std::cout << "Con la tecla F la cara FRONT" << std::endl;
    std::cout << "Con la tecla B la cara BACK" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Se puede mover la camara con las flechas direccionales" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Con la tecla K Se resolvera el cubo" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwSetKeyCallback(window, processInputRubik);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 R = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
        cameraFront = glm::vec3(R * glm::vec4(0, 0, -1, 0));
        cameraUp = glm::vec3(R * glm::vec4(0, 1, 0, 0));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        if(condition_input == "R")
            condition_input = c.animation_R();
        if(condition_input == "U")
            condition_input = c.animation_U();
        if(condition_input == "D")
            condition_input = c.animation_D();
        if(condition_input == "F")
            condition_input = c.animation_F();
        if(condition_input == "B")
            condition_input = c.animation_B();
        if(condition_input == "L")
            condition_input = c.animation_L();
        if(condition_input == "A")
            condition_input = c.aplicar_solucion(solution_cubo);
        
        c.draw(window);

        glUseProgram(shaderProgram);
        
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera.Position[0]);

        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

        glBindVertexArray(VAO);

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

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void processInputRubik(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw -= cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("R");
        condition_input = "R";
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("L");
        condition_input = "L";
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("U");
        condition_input = "U";
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("D");
        condition_input = "D";
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("F");
        condition_input = "F";
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && condition_input == "N") {
        reg_mov.push_back("B");
        condition_input = "B";
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && condition_input == "N") {
        // ...
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && condition_input == "N") {
        condition_input = "A";
        std::string tempo = to_cube_not(reg_mov);
        reg_mov.clear();
        solution_cubo = get_solution(tempo);
        std::cout << std::endl;
    }
}
