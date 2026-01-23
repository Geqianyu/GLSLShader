#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/plane.h"
#include "common/texture.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.1f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint check_board_texture = 0;
GLuint samplers[2] = { 0, 0 };

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitTextures();
void TerminateTextures();
void InitSamplers();
void TerminateSamplers();

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

    window = glfwCreateWindow(800, 600, "Chapter32", nullptr, nullptr);
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
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    // 初始化纹理
    InitTextures();

    // 初始化采样器
    InitSamplers();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(1, 0, 0));
        glm::mat4 model = glm::translate(rot, glm::vec3(-5.01f, 0.f, 0.f));
        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        glBindSampler(0, samplers[1]);
        plane->Render();

        model = glm::translate(rot, glm::vec3(5.01f, 0.f, 0.f));
        mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        glBindSampler(0, samplers[0]);
        plane->Render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateSamplers();
    TerminateTextures();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter32/use_sampler.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter32/use_sampler.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 20.0f, 0.0f, 1.0f));
    program.SetUniform("u_light.L", glm::vec3(1.0f));
    program.SetUniform("u_light.La", glm::vec3(0.2f));
    program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
    program.SetUniform("u_material.shininess", 100.0f);
}

void InitGeometry()
{
    plane = std::make_unique<glsl_shader::Plane>(10.0f, 10.0f, 1, 1);
}

void TerminateGeometry()
{
    plane.release();
}

void InitTextures()
{
    GLint width = 128;
    GLint height = 128;
    GLint check_size = 4;
    std::vector<GLubyte> data(width * height * 4);
    for (int r = 0; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            GLubyte color = 0;
            if (((c / check_size) + (r / check_size)) % 2 == 0)
            {
                color = 0;
            }
            else
            {
                color = 255;
            }
            data[(r * width + c) * 4 + 0] = color;
            data[(r * width + c) * 4 + 1] = color;
            data[(r * width + c) * 4 + 2] = color;
            data[(r * width + c) * 4 + 3] = 255;
        }
    }

    glGenTextures(1, &check_board_texture);
    glBindTexture(GL_TEXTURE_2D, check_board_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, check_board_texture);
}

void TerminateTextures()
{
    glDeleteTextures(1, &check_board_texture);
}

void InitSamplers()
{
    glGenSamplers(2, samplers);
    GLuint linear_sampler = samplers[0];
    GLuint nearest_sampler = samplers[1];

    glSamplerParameteri(linear_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linear_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glSamplerParameteri(nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void TerminateSamplers()
{
    glDeleteSamplers(2, samplers);
}