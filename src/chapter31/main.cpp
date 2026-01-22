#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/cube.h"
#include "common/obj_mesh.h"
#include "common/texture.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Cube> cube;
std::unique_ptr<glsl_shader::ObjMesh> obj_mesh;
glm::vec3 camera_position = glm::vec3(0.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint frame_buffer_obj = 0;
GLuint render_texture = 0;
GLuint depth_buffer = 0;
GLuint spot_texture = 0;
float last_time = 0.0f;
float angle = glm::radians(140.0f);

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitTextures();
void TerminateTextures();
void InitFrameBuffer();
void TerminateFrameBuffer();
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

    window = glfwCreateWindow(800, 600, "Chapter31", nullptr, nullptr);
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

    // 初始化 frame buffer
    InitFrameBuffer();

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Update();

        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);
        glViewport(0, 0, 512, 512);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.SetUniform("u_texture", 1);

        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.3f, 100.0f);

        program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
        program.SetUniform("u_material.shininess", 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection * mv);
        obj_mesh->Render();

        glFlush();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 800, 600);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.SetUniform("u_texture", 0);

        glm::vec3 camera_position = glm::vec3(2.0f * glm::cos(angle), 1.5f, 2.0f * glm::sin(angle));
        view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.3f, 100.0f);

        program.SetUniform("u_light.position_in_view", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        program.SetUniform("u_material.Ks", 0.0f, 0.0f, 0.0f);
        program.SetUniform("u_material.shininess", 1.0f);

        model = glm::mat4(1.0f);
        mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        program.SetUniform("u_mvp_matrix", projection* mv);
        cube->Render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateFrameBuffer();
    TerminateTextures();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter31/render_to_texture.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter31/render_to_texture.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.L", glm::vec3(1.0f));
    program.SetUniform("u_light.La", glm::vec3(0.15f));
}

void InitGeometry()
{
    obj_mesh = glsl_shader::ObjMesh::Load("../../assets/models/spot_triangulated.obj");
    cube = std::make_unique<glsl_shader::Cube>();
}

void TerminateGeometry()
{
    cube.release();
    obj_mesh.release();
}

void InitTextures()
{
    spot_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/spot_texture.png");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, spot_texture);
}

void TerminateTextures()
{
    glDeleteTextures(1, &spot_texture);
}

void InitFrameBuffer()
{
    glGenFramebuffers(1, &frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);

    glGenTextures(1, &render_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is complete" << std::endl;
    }
    else {
        std::cout << "Framebuffer error: " << result << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerminateFrameBuffer()
{
    glDeleteRenderbuffers(1, &depth_buffer);
    glDeleteTextures(1, &render_texture);
    glDeleteFramebuffers(1, &frame_buffer_obj);
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
}