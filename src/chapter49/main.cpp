#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"

#include "common/teapot_patch.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>

GLFWwindow* window = nullptr;
std::unique_ptr<glsl_shader::TeapotPatch> teapot_patch;
glsl_shader::GLSLProgram program;
glm::mat4 viewport = glm::mat4
                    (
                        glm::vec4(400.0f, 0.0f, 0.0f, 0.0f),
                        glm::vec4(0.0f, 300.0f, 0.0f, 0.0f),
                        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                        glm::vec4(400.0f, 300.0f, 0.0f, 1.0f)
                    );
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);

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

    window = glfwCreateWindow(800, 600, "Chapter49", nullptr, nullptr);
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

        program.Use();

        glm::vec3 camera_position(0.0f, 1.0f, 6.25f);
        view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 mv = view * model;
        program.SetUniform("u_model_view_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_viewport_matrix", viewport);

        teapot_patch->Render();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, -5.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

         mv = view * model;
        program.SetUniform("u_model_view_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_viewport_matrix", viewport);

        teapot_patch->Render();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, 0.0f, -10.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        mv = view * model;
        program.SetUniform("u_model_view_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_viewport_matrix", viewport);

        teapot_patch->Render();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(17.0f, 0.0f, -20.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        mv = view * model;
        program.SetUniform("u_model_view_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_viewport_matrix", viewport);

        teapot_patch->Render();

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
    program.CompileShader("../../assets/shaders/chapter49/tessellating_based_depth.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter49/tessellating_based_depth.tcs.glsl");
    program.CompileShader("../../assets/shaders/chapter49/tessellating_based_depth.tes.glsl");
    program.CompileShader("../../assets/shaders/chapter49/tessellating_based_depth.gs.glsl");
    program.CompileShader("../../assets/shaders/chapter49/tessellating_based_depth.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_min_tess_level", 2);
    program.SetUniform("u_max_tess_level", 15);
    program.SetUniform("u_max_depth", 20.0f);
    program.SetUniform("u_min_depth", 2.0f);
    program.SetUniform("u_line_width", 0.8f);
    program.SetUniform("u_line_color", glm::vec4(0.05f, 0.0f, 0.05f, 1.0f));
    program.SetUniform("u_light_position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_light_intensity", glm::vec3(1.0f, 1.0f, 1.0f));
    program.SetUniform("u_Kd", glm::vec3(0.9f, 0.9f, 1.0f));
}

void InitGeometry()
{
    teapot_patch = std::make_unique<glsl_shader::TeapotPatch>();

    glPatchParameteri(GL_PATCH_VERTICES, 16);
}

void TerminateGeometry()
{
    teapot_patch.release();
}