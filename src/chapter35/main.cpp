#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/plane.h"
#include "common/torus.h"
#include "common/teapot.h"

#include <iostream>
#include <memory>
#include <sstream>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::Torus> torus;
std::unique_ptr<glsl_shader::Teapot> teapot;
glm::vec3 camera_position = glm::vec3(0.0f);
glm::vec4 light_position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint frame_buffer_obj = 0;
GLuint render_texture = 0;
GLuint depth_buffer = 0;
GLuint intermediate_frame_buffer_obj = 0;
GLuint intermediate_render_texture = 0;
GLuint intermediate_depth_buffer = 0;
GLuint full_screen_quad_vbo = 0;
GLuint full_screen_quad_uv_vbo = 0;
GLuint full_screen_quad_vao = 0;
float last_time = 0.0f;
float angle = glm::pi<float>() * 0.25f;
float weights[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float sum = 0.0f;
float sigma2 = 8.0f;

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitFrameBuffer();
void TerminateFrameBuffer();
void Update();
void Pass1();
void Pass2();
void Pass3();
float CalculateGaussian(float x, float sigma2);

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

    window = glfwCreateWindow(800, 600, "Chapter35", nullptr, nullptr);
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

    // 设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    // 初始化 frame buffer
    InitFrameBuffer();

    last_time = static_cast<float>(glfwGetTime());

    weights[0] = CalculateGaussian(0, sigma2);
    sum = weights[0];
    for (int i = 1; i < 5; i++)
    {
        weights[i] = CalculateGaussian(static_cast<float>(i), sigma2);
        sum += 2.0f * weights[i];
    }

    for (int i = 0; i < 5; i++)
    {
        std::stringstream uniform_name;
        uniform_name << "u_weight[" << i << "]";
        float val = weights[i] / sum;
        program.SetUniform(uniform_name.str().c_str(), val);
    }

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Update();
        Pass1();
        glFlush();
        Pass2();
        glFlush();
        Pass3();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateFrameBuffer();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter35/gaussian_blur.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter35/gaussian_blur.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.L", glm::vec3(1.0f));
    program.SetUniform("u_light.La", glm::vec3(0.2f));
}

void InitGeometry()
{
    plane = std::make_unique<glsl_shader::Plane>(50.0f, 50.0f, 1, 1);
    torus = std::make_unique<glsl_shader::Torus>(0.7f * 1.5f, 0.3f * 1.5f, 50, 50);
    teapot = std::make_unique<glsl_shader::Teapot>(14, glm::mat4(1.0f));

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

    glGenBuffers(1, &full_screen_quad_vbo);
    glGenBuffers(1, &full_screen_quad_uv_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glGenVertexArrays(1, &full_screen_quad_vao);
    glBindVertexArray(full_screen_quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_uv_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TerminateGeometry()
{
    plane.release();
    torus.release();
    teapot.release();
    glDeleteBuffers(1, &full_screen_quad_vbo);
    glDeleteBuffers(1, &full_screen_quad_uv_vbo);
    glDeleteVertexArrays(1, &full_screen_quad_vao);
}

void InitFrameBuffer()
{
    glGenFramebuffers(1, &frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);

    glGenTextures(1, &render_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);

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

    glGenFramebuffers(1, &intermediate_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediate_frame_buffer_obj);

    glGenTextures(1, &intermediate_render_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediate_render_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 800, 600);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediate_render_texture, 0);

    glGenRenderbuffers(1, &intermediate_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, intermediate_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, intermediate_depth_buffer);

    glDrawBuffers(1, draw_buffers);

    result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Intermediate framebuffer is complete" << std::endl;
    }
    else {
        std::cout << "Intermediate framebuffer error: " << result << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerminateFrameBuffer()
{
    glDeleteRenderbuffers(1, &depth_buffer);
    glDeleteTextures(1, &render_texture);
    glDeleteFramebuffers(1, &frame_buffer_obj);
    glDeleteRenderbuffers(1, &intermediate_depth_buffer);
    glDeleteTextures(1, &intermediate_render_texture);
    glDeleteFramebuffers(1, &intermediate_frame_buffer_obj);
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

void Pass1()
{
    program.SetUniform("u_pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(glm::vec3(7.0f * glm::cos(angle), 4.0f, 7.0f * glm::sin(angle)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);

    program.SetUniform("u_light.position_in_view", light_position);
    program.SetUniform("u_material.Kd", 0.9f, 0.9f, 0.9f);
    program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
    program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
    program.SetUniform("u_material.shininess", 100.0f);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    teapot->Render();

    program.SetUniform("u_material.Kd", 0.4f, 0.4f, 0.4f);
    program.SetUniform("u_material.Ks", 0.0f, 0.0f, 0.0f);
    program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
    program.SetUniform("u_material.shininess", 1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    program.SetUniform("u_light.position_in_view", light_position);
    program.SetUniform("u_material.Kd", 0.9f, 0.5f, 0.2f);
    program.SetUniform("u_material.Ks", 0.95f, 0.95f, 0.95f);
    program.SetUniform("u_material.Ka", 0.1f, 0.1f, 0.1f);
    program.SetUniform("u_material.shininess", 100.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    torus->Render();
}

void Pass2()
{
    program.SetUniform("u_pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, intermediate_frame_buffer_obj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_texture);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Pass3()
{
    program.SetUniform("u_pass", 3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediate_render_texture);

    glClear(GL_COLOR_BUFFER_BIT);

    model = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float CalculateGaussian(float x, float sigma2)
{
    float coeff = 1.0f / (glm::two_pi<float>() * sigma2);
    float exponent = -(x * x) / (2.0f * sigma2);
    return coeff * glm::exp(exponent);
}