#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/teapot.h"
#include "common/torus.h"
#include "common/plane.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::Torus> torus;
std::unique_ptr<glsl_shader::Teapot> teapot;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::ortho(-0.4f * 5.0f, 0.4f * 5.0f, -0.3f * 5.0f, 0.3f * 5.0f, 0.1f, 100.0f);
float angle = glm::pi<float>() / 2.0f;
float last_time = 0.0f;
GLuint deferred_frame_buffer_obj = 0;
GLuint deferred_depth_buffer = 0;
GLuint deferred_position_texture = 0;
GLuint deferred_normal_texture = 0;
GLuint deferred_color_texture = 0;
GLuint quad_vao = 0;
GLuint quad_vertices = 0;
GLuint quad_uvs = 0;

void LoadShaderFromSourceCode();
void CreateGBufferTexture(GLenum texture_unit, GLenum format, GLuint& texture);
void InitGeometry();
void TerminateGeometry();
void InitFrameBufferObject();
void TerminateFrameBufferObject();
void Update();
void Pass1();
void Pass2();

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

    window = glfwCreateWindow(800, 600, "Chapter40", nullptr, nullptr);
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

    // 开启深度检测
    glEnable(GL_DEPTH_TEST);

    // 设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    // 初始化帧缓冲对象
    InitFrameBufferObject();

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Update();

        Pass1();
        Pass2();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateFrameBufferObject();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter40/deferred_shading.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter40/deferred_shading.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.L", glm::vec3(1.0f));
}

void CreateGBufferTexture(GLenum texture_unit, GLenum format, GLuint& texture)
{
    glActiveTexture(texture_unit);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, format, 800, 600);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

void InitGeometry()
{
    plane = std::make_unique<glsl_shader::Plane>(50.0f, 50.0f, 1, 1);
    torus = std::make_unique<glsl_shader::Torus>(0.7f * 1.5f, 0.3f * 1.5f, 50, 50);
    teapot = std::make_unique<glsl_shader::Teapot>(14, glm::mat4(1.0));

    GLfloat vertices[]
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };
    GLfloat uvs[]
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    glGenBuffers(1, &quad_vertices);
    glGenBuffers(1, &quad_uvs);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, quad_uvs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, quad_uvs);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void TerminateGeometry()
{
    plane.release();
    torus.release();
    teapot.release();
    glDeleteBuffers(1, &quad_vertices);
    glDeleteBuffers(1, &quad_uvs);
    glDeleteVertexArrays(1, &quad_vao);
}

void InitFrameBufferObject()
{
    glGenFramebuffers(1, &deferred_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer_obj);

    glGenRenderbuffers(1, &deferred_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, deferred_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);

    CreateGBufferTexture(GL_TEXTURE0, GL_RGB32F, deferred_position_texture);
    CreateGBufferTexture(GL_TEXTURE1, GL_RGB32F, deferred_normal_texture);
    CreateGBufferTexture(GL_TEXTURE2, GL_RGB8, deferred_color_texture);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, deferred_depth_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferred_position_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferred_normal_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferred_color_texture, 0);

    GLenum draw_buffers[]
    {
        GL_NONE,
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };
    glDrawBuffers(4, draw_buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerminateFrameBufferObject()
{
    glDeleteFramebuffers(1, &deferred_frame_buffer_obj);
    glDeleteRenderbuffers(1, &deferred_depth_buffer);
    glDeleteTextures(1, &deferred_position_texture);
    glDeleteTextures(1, &deferred_normal_texture);
    glDeleteTextures(1, &deferred_color_texture);
}

void Update()
{
    float current_time = static_cast<float>(glfwGetTime());
    float delta_time = current_time - last_time;
    last_time = current_time;
    angle += glm::pi<float>() / 8.0f * delta_time;
    if (angle > glm::two_pi<float>())
    {
        angle -= glm::two_pi<float>();
    }
}

void Pass1()
{
    program.SetUniform("u_pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer_obj);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(glm::vec3(7.0f * glm::cos(angle), 4.0f, 7.0f * glm::sin(angle)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);

    program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_material.Kd", 0.9f, 0.9f, 0.9f);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    teapot->Render();

    program.SetUniform("u_material.Kd", 0.4f, 0.4f, 0.4f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_material.Kd", 0.9f, 0.5f, 0.2f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    torus->Render();

    glFinish();
}

void Pass2()
{
    program.SetUniform("u_pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    view = glm::mat4(1.0);
    model = glm::mat4(1.0);
    projection = glm::mat4(1.0);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}