#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/texture.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint points_vao = 0;
GLuint points_vbo = 0;
GLuint sprite_texture = 0;

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitTexture();
void TerminateTexture();

int main()
{
    // 初始化 GLFW
    if (!glfwInit())
    {
        std::cerr << "初始化 GLFW 失败" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 设置 GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(800, 600, "Chapter43", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "创建窗口失败" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress)))
    {
        std::cerr << "初始化 GLAD 失败" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 输出信息
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "OpenGL Version: " << version << std::endl;
    std::cout << "Shading Language Version: " << glsl_version << std::endl;
    std::cout << "OpenGL Version (parsed): " << major << "." << minor << std::endl;

    // 输出支持的扩展信息
    GLint extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensions);
    std::cout << "Supported Extensions:" << std::endl;
    for (int i = 0; i < extensions; ++i)
    {
        std::cout << "    " << glGetStringi(GL_EXTENSIONS, i) << std::endl;
    }

    // 设置视口大小
    glViewport(0, 0, 800, 600);

    // 启动深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    // 初始化纹理
    InitTexture();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 camera_position(0.0f, 0.0f, 3.0f);
        view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::mat4(1.0f);
        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_projection_matrix", projection);

        glBindVertexArray(points_vao);
        glDrawArrays(GL_POINTS, 0, 50);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateGeometry();
    TerminateTexture();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter43/point_sprite.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter43/point_sprite.gs.glsl");
    program.CompileShader("../../assets/shaders/chapter43/point_sprite.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_sprite_texture", 0);
    program.SetUniform("u_size", 0.15f);
}

void InitGeometry()
{
    float* locations = new float[50 * 3];
    srand((unsigned int)time(0));
    for (int i = 0; i < 50; ++i)
    {
        glm::vec3 position((static_cast<float>(rand()) / RAND_MAX * 2.0f) - 1.0f, (static_cast<float>(rand()) / RAND_MAX * 2.0f) - 1.0f, (static_cast<float>(rand()) / RAND_MAX * 2.0f) - 1.0f);
        locations[i * 3] = position.x;
        locations[i * 3 + 1] = position.y;
        locations[i * 3 + 2] = position.z;
    }

    glGenBuffers(1, &points_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, 50 * 3 * sizeof(float), locations, GL_STATIC_DRAW);

    delete[] locations;
    locations = nullptr;

    glGenVertexArrays(1, &points_vao);
    glBindVertexArray(points_vao);

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void TerminateGeometry()
{
    glDeleteBuffers(1, &points_vbo);
    glDeleteVertexArrays(1, &points_vao);
}

void InitTexture()
{
    sprite_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/flower.png");
}

void TerminateTexture()
{
    glDeleteTextures(1, &sprite_texture);
}