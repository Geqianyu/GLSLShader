#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
glsl_shader::GLSLProgram solid_program;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::ortho(-0.4f * 3.5f, 0.4f * 3.5f, -0.3f * 3.5f, 0.3f * 3.5f, 0.1f, 100.0f);
GLuint vao = 0;
GLuint vbo = 0;

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();

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

    window = glfwCreateWindow(800, 600, "Chapter46", nullptr, nullptr);
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

    // 设置点的大小
    glPointSize(10.0f);

    // 设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 camera_position(0.0f, 0.0f, 1.5f);
        view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 mv = view * model;
        program.Use();
        program.SetUniform("u_mvp_matrix", projection * mv);
        solid_program.Use();
        solid_program.SetUniform("u_mvp_matrix", projection * mv);

        glBindVertexArray(vao);

        program.Use();
        glDrawArrays(GL_PATCHES, 0, 4);

        solid_program.Use();
        glDrawArrays(GL_POINTS, 0, 4);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter46/tessellating_curve.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter46/tessellating_curve.tcs.glsl");
    program.CompileShader("../../assets/shaders/chapter46/tessellating_curve.tes.glsl");
    program.CompileShader("../../assets/shaders/chapter46/tessellating_curve.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_segments_num", 50);
    program.SetUniform("u_strips_num", 1);
    program.SetUniform("u_line_color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));

    solid_program.CompileShader("../../assets/shaders/chapter46/solid.vs.glsl");
    solid_program.CompileShader("../../assets/shaders/chapter46/solid.fs.glsl");
    solid_program.Link();
    solid_program.Use();
    solid_program.PrintActiveAttribs();
    solid_program.PrintActiveUniformBlocks();
    solid_program.PrintActiveUniforms();

    solid_program.SetUniform("color", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
}

void InitGeometry()
{
    float v[]
    {
        -1.0f, -1.0f,
        -0.5f,  1.0f,
         0.5f, -1.0f,
         1.0f,  1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void TerminateGeometry()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}