#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/plane.h"
#include "common/sphere.h"
#include "common/teapot.h"

#include <iostream>
#include <memory>
#include <sstream>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::Sphere> sphere;
std::unique_ptr<glsl_shader::Teapot> teapot;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint hdr_frame_buffer_obj = 0;
GLuint hdr_texture = 0;
GLuint depth_buffer = 0;
GLuint blur_frame_buffer_obj = 0;
GLuint blur_texture1 = 0;
GLuint blur_texture2 = 0;
GLuint full_screen_quad_vbo = 0;
GLuint full_screen_quad_uv_vbo = 0;
GLuint full_screen_quad_vao = 0;
GLuint linear_sampler = 0;
GLuint neaset_sampler = 0;
float weights[10] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float sigma2 = 25.0f;

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void InitFrameBuffer();
void TerminateFrameBuffer();
void InitSamplers();
void TerminateSamplers();
void Pass1();
void Pass2();
void Pass3();
void Pass4();
void Pass5();
void ComputeLogAveLuminance();
float CalculateGuass(float x, float sigma2);

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

    window = glfwCreateWindow(800, 600, "Chapter37", nullptr, nullptr);
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

    InitSamplers();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Pass1();
        ComputeLogAveLuminance();
        Pass2();
        Pass3();
        Pass4();
        Pass5();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateSamplers();
    TerminateFrameBuffer();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter37/bloom_effect.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter37/bloom_effect.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_lights[0].L", glm::vec3(0.6f));
    program.SetUniform("u_lights[0].La", glm::vec3(0.2f));
    program.SetUniform("u_lights[1].L", glm::vec3(0.6f));
    program.SetUniform("u_lights[1].La", glm::vec3(0.2f));
    program.SetUniform("u_lights[2].L", glm::vec3(0.6f));
    program.SetUniform("u_lights[2].La", glm::vec3(0.2f));

    program.SetUniform("u_lum_thresh", 1.7f);

    weights[0] = CalculateGuass(0, sigma2);
    float sum = weights[0];
    for (int i = 1; i < 10; ++i)
    {
        weights[i] = CalculateGuass(static_cast<float>(i), sigma2);
        sum += 2.0f * weights[i];
    }

    for (int i = 0; i < 10; ++i)
    {
        std::stringstream uniform_name;
        uniform_name << "u_weights[" << i << "]";
        float val = weights[i] / sum;
        program.SetUniform(uniform_name.str().c_str(), val);
    }
}

void InitGeometry()
{
    plane = std::make_unique<glsl_shader::Plane>(20.0f, 10.0f, 1, 1);
    sphere = std::make_unique<glsl_shader::Sphere>(2.0f, 50, 50);
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void TerminateGeometry()
{
    plane.release();
    sphere.release();
    teapot.release();
    glDeleteBuffers(1, &full_screen_quad_vbo);
    glDeleteBuffers(1, &full_screen_quad_uv_vbo);
    glDeleteVertexArrays(1, &full_screen_quad_vao);
}

void InitFrameBuffer()
{
    glGenFramebuffers(1, &hdr_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_frame_buffer_obj);

    glGenTextures(1, &hdr_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 800, 600);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_texture, 0);

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

    glGenFramebuffers(1, &blur_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_frame_buffer_obj);

    glGenTextures(1, &blur_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blur_texture1);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 100, 75);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_texture1, 0);
    glDrawBuffers(1, draw_buffers);

    glGenTextures(1, &blur_texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, blur_texture2);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 100, 75);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerminateFrameBuffer()
{
    glDeleteRenderbuffers(1, &depth_buffer);
    glDeleteTextures(1, &hdr_texture);
    glDeleteFramebuffers(1, &hdr_frame_buffer_obj);
    glDeleteTextures(1, &blur_texture1);
    glDeleteTextures(1, &blur_texture2);
    glDeleteFramebuffers(1, &blur_frame_buffer_obj);
}

void InitSamplers()
{
    glGenSamplers(1, &linear_sampler);
    glGenSamplers(1, &neaset_sampler);

    GLfloat border[] = { 0.0f,0.0f,0.0f,0.0f };
    glSamplerParameteri(neaset_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(neaset_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(neaset_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(neaset_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(neaset_sampler, GL_TEXTURE_BORDER_COLOR, border);

    glSamplerParameteri(linear_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linear_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(linear_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(linear_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(linear_sampler, GL_TEXTURE_BORDER_COLOR, border);

    glBindSampler(0, neaset_sampler);
    glBindSampler(1, neaset_sampler);
    glBindSampler(2, neaset_sampler);
}

void TerminateSamplers()
{
    glDeleteSamplers(1, &linear_sampler);
    glDeleteSamplers(1, &neaset_sampler);
}

void Pass1()
{
    program.SetUniform("u_pass", 1);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, 800, 600);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_frame_buffer_obj);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(glm::vec3(2.0f, 0.0f, 14.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.3f, 100.0f);

    glm::vec3 intense = glm::vec3(1.0f);
    program.SetUniform("u_lights[0].L", intense);
    program.SetUniform("u_lights[1].L", intense);
    program.SetUniform("u_lights[2].L", intense);

    glm::vec4 light_position = glm::vec4(0.0f, 4.0f, 2.5f, 1.0f);
    light_position.x = -7.0f;
    program.SetUniform("u_lights[0].position_in_view", view * light_position);
    light_position.x = 0.0f;
    program.SetUniform("u_lights[1].position_in_view", view * light_position);
    light_position.x = 7.0f;
    program.SetUniform("u_lights[2].position_in_view", view * light_position);

    program.SetUniform("u_material.Kd", 0.9f, 0.3f, 0.2f);
    program.SetUniform("u_material.Ks", 1.0f, 1.0f, 1.0f);
    program.SetUniform("u_material.Ka", 0.2f, 0.2f, 0.2f);
    program.SetUniform("u_material.shininess", 20.0f);

    model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    program.SetUniform("u_material.Kd", glm::vec3(0.4f, 0.9f, 0.4f));
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -3.0f, 2.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    sphere->Render();

    program.SetUniform("u_material.Kd", glm::vec3(0.4f, 0.4f, 0.9f));
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -5.0f, 1.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    teapot->Render();
}

void Pass2()
{
    program.SetUniform("u_pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, blur_frame_buffer_obj);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_texture1, 0);
    glViewport(0, 0, 100, 75);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    view = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Pass3()
{
    program.SetUniform("u_pass", 3);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_texture2, 0);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Pass4()
{
    program.SetUniform("u_pass", 4);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_texture1, 0);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Pass5()
{
    program.SetUniform("u_pass", 5);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 800, 600);

    glBindSampler(1, linear_sampler);

    glBindVertexArray(full_screen_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindSampler(1, neaset_sampler);
}

void ComputeLogAveLuminance()
{
    int size = 800 * 600;
    std::vector<GLfloat> texture_data(size * 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texture_data.data());
    float sum = 0.0f;
    for (int i = 0; i < size; ++i)
    {
        float lum = glm::dot(glm::vec3(texture_data[i * 3 + 0], texture_data[i * 3 + 1], texture_data[i * 3 + 2]), glm::vec3(0.2126f, 0.7152f, 0.0722f));
        sum += std::logf(lum + 0.00001f);
    }
    program.SetUniform("u_ave_lum", std::expf(sum / size));
}

float CalculateGuass(float x, float sigma2)
{
    float coeff = 1.0f / (glm::two_pi<float>() * sigma2);
    float exponent = -(x * x) / (2.0 * sigma2);
    return coeff * std::exp(exponent);
}