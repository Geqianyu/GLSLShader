#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/plane.h"
#include "common/teapot.h"
#include "common/texture.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::Teapot> teapot;
glm::vec3 camera_position = glm::vec3(0.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint flower_texture = 0;
float last_time = 0.0f;
float angle = glm::radians(90.0f);

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitTextures();
void TerminateTextures();
void Update();

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

    window = glfwCreateWindow(800, 600, "Chapter30", nullptr, nullptr);
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

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update();

        program.Use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 mv = view * model;
        program.SetUniform("u_model_matrix", model);
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_material.Kd", 0.5f, 0.2f, 0.1f);
        program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
        program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
        program.SetUniform("u_material.shininess", 100.0f);
        teapot->Render();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
        mv = view * model;
        program.SetUniform("u_model_matrix", model);
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_material.Kd", 0.4f, 0.4f, 0.4f);
        program.SetUniform("u_material.Ks", 0.0f, 0.0f, 0.0f);
        program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
        program.SetUniform("u_material.shininess", 1.0f);
        plane->Render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateTextures();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
   program.CompileShader("../../assets/shaders/chapter30/projector_texture.vs.glsl");
   program.CompileShader("../../assets/shaders/chapter30/projector_texture.fs.glsl");
   program.Link();
   program.Use();
   program.PrintActiveAttribs();
   program.PrintActiveUniformBlocks();
   program.PrintActiveUniforms();

    program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_light.L", glm::vec3(1.0f));
    program.SetUniform("u_light.La", glm::vec3(0.2f));

    glm::mat4 projector_matrix = glm::mat4(1.0f);

    glm::vec3 projctor_position = glm::vec3(2.0f, 5.0f, 5.0f);
    glm::vec3 projctor_at = glm::vec3(-2.0f, -4.0f, 0.0f);
    glm::vec3 projctor_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 projctor_view = glm::lookAt(projctor_position, projctor_at, projctor_up);
    glm::mat4 projctor_projction = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
    glm::mat4 bias = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    bias = glm::scale(bias, glm::vec3(0.5f));
    program.SetUniform("u_projector_matrix", bias * projctor_projction * projctor_view);
}

void InitGeometry()
{
    teapot = std::make_unique<glsl_shader::Teapot>(14, glm::mat4(1.0f));
    plane = std::make_unique<glsl_shader::Plane>(100.0f, 100.0f, 1, 1);
}

void TerminateGeometry()
{
    plane.release();
    teapot.release();
}

void InitTextures()
{
    flower_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/flower.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, flower_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void TerminateTextures()
{
    glDeleteTextures(1, &flower_texture);
}

void Update()
{
    float current_time = static_cast<float>(glfwGetTime());
    float delta_time = current_time - last_time;
    last_time = current_time;
    angle += glm::pi<float>() * 0.125f * delta_time;
    if (angle > glm::two_pi<float>())
    {
        angle -= glm::two_pi<float>();
    }

    camera_position = glm::vec3(7.0f * glm::cos(angle), 2.0f, 7.0f * glm::sin(angle));
    view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}