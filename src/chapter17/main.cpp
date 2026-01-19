#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/teapot.h"
#include "common/plane.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Teapot> teapot;
std::unique_ptr<glsl_shader::Plane> plane;
float angle = 0.957283f;
float last_time = 0.0f;

void LoadShaderFromSourceCode();

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

    window = glfwCreateWindow(800, 600, "Chapter17", nullptr, nullptr);
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

    // 启动深度检测
    glEnable(GL_DEPTH_TEST);

    // 设置视口大小
    glViewport(0, 0, 800, 600);

    // 设置背景颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 设置平面着色时使用第一个顶点的颜色
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    teapot = std::make_unique<glsl_shader::Teapot>(14, glm::mat4(1.0f));
    plane = std::make_unique<glsl_shader::Plane>(50.0f, 50.0f, 1, 1);

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float current_time = static_cast<float>(glfwGetTime());
        angle += 0.25f * (current_time - last_time);
        last_time = current_time;
        if (angle > glm::two_pi<float>())
        {
            angle -= glm::two_pi<float>();
        }

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);

        program.SetUniform("u_light.position_in_view", view * glm::vec4(10.0f * glm::cos(angle), 3.0f, 10.0f * glm::sin(angle), 1.0f));

        program.SetUniform("u_material.Kd", 0.9f, 0.5f, 0.3f);
        program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
        program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
        program.SetUniform("u_material.shininess", 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        teapot->Render();

        program.SetUniform("u_material.Kd", 0.7f, 0.7f, 0.7f);
        program.SetUniform("u_material.Ks", 0.9f, 0.9f, 0.9f);
        program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
        program.SetUniform("u_material.shininess", 180.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.45f, 0.0f));
        mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        plane->Render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    plane.release();
    teapot.release();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter17/blinn_phong.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter17/blinn_phong.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.L", glm::vec3(0.9f));
    program.SetUniform("u_light.La", glm::vec3(0.1f));
}