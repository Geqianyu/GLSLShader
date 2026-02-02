#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/obj_mesh.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::ortho(-0.4f * 1.5f, 0.4f * 1.5f, -0.3f * 1.5f, 0.3f * 1.5f, 0.1f, 100.0f);
glm::mat4 viewport = glm::mat4
(
    glm::vec4(400.0f, 0.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 300.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
    glm::vec4(400.0f, 300.0f, 0.0f, 1.0f)
);
std::unique_ptr<glsl_shader::ObjMesh> obj_mesh;

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

    window = glfwCreateWindow(800, 600, "Chapter44", nullptr, nullptr);
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

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 camera_position(0.0f, 0.0f, 3.0f);
        view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_viewport_matrix", viewport);
        obj_mesh->Render();

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
    program.CompileShader("../../assets/shaders/chapter44/shade_wire.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter44/shade_wire.gs.glsl");
    program.CompileShader("../../assets/shaders/chapter44/shade_wire.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_line.width", 0.75f);
    program.SetUniform("u_line.color", glm::vec4(0.05f, 0.0f, 0.05f, 1.0f));
    program.SetUniform("u_material.Kd", 0.7f, 0.7f, 0.7f);
    program.SetUniform("u_light.position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_material.Ka", 0.2f, 0.2f, 0.2f);
    program.SetUniform("u_light.intensity", 1.0f, 1.0f, 1.0f);
    program.SetUniform("u_material.Ks", 0.8f, 0.8f, 0.8f);
    program.SetUniform("u_material.shininess", 100.0f);
}

void InitGeometry()
{
    obj_mesh = glsl_shader::ObjMesh::Load("../../assets/models/bs_ears.obj");
}

void TerminateGeometry()
{
    obj_mesh.release();
}