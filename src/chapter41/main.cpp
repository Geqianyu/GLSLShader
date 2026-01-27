#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/texture.h"
#include "common/obj_mesh.h"
#include "common/plane.h"
#include "common/random.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::ObjMesh> obj_mesh;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 scene_projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 0.3f, 100.0f);
glm::mat4 projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint deferred_frame_buffer_obj = 0;
GLuint deferred_depth_buffer = 0;
GLuint deferred_position_texture = 0;
GLuint deferred_normal_texture = 0;
GLuint deferred_color_texture = 0;
GLuint ao_textures[2] = { 0, 0 };
GLuint ssao_frame_buffer_obj = 0;
GLuint quad_vao = 0;
GLuint quad_vertices = 0;
GLuint quad_uvs = 0;
GLuint wood_texture = 0;
GLuint brick_texture = 0;
GLuint random_texture = 0;
glsl_shader::Random random;

void LoadShaderFromSourceCode();
void CreateGBufferTexture(GLenum texture_unit, GLenum format, GLuint& texture);
void InitGeometry();
void TerminateGeometry();
void InitTextures();
void TerminateTextures();
void InitFrameBufferObject();
void TerminateFrameBufferObject();
void BuildKernel();
GLuint BuildRandomTexture();
void Pass1();
void Pass2();
void Pass3();
void Pass4();
void DrawScene();
void DrawQuad();

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

    window = glfwCreateWindow(800, 600, "Chapter41", nullptr, nullptr);
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

    // 初始化纹理
    InitTextures();

    // 创建采样核
    BuildKernel();

    // 初始化帧缓冲对象
    InitFrameBufferObject();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Pass1();
        Pass2();
        Pass3();
        Pass4();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateFrameBufferObject();
    TerminateTextures();
    TerminateGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter41/ssao.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter41/ssao.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_light.L", glm::vec3(0.3f));
    program.SetUniform("u_light.La", glm::vec3(0.5f));
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
    plane = std::make_unique<glsl_shader::Plane>(10.0f, 10.0f, 1, 1, 10.0f, 7.0f);
    obj_mesh = glsl_shader::ObjMesh::Load("../../assets/models/dragon.obj");

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
    obj_mesh.release();
    glDeleteBuffers(1, &quad_vertices);
    glDeleteBuffers(1, &quad_uvs);
    glDeleteVertexArrays(1, &quad_vao);
}

void InitTextures()
{
    wood_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/hardwood2_diffuse.jpg");
    brick_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/brick1.jpg");

    // 创建随机旋转纹理
    random_texture = BuildRandomTexture();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, random_texture);
}

void TerminateTextures()
{
    glDeleteTextures(1, &wood_texture);
    glDeleteTextures(1, &brick_texture);
    glDeleteTextures(1, &random_texture);
}

void InitFrameBufferObject()
{
    CreateGBufferTexture(GL_TEXTURE0, GL_RGB32F, deferred_position_texture);
    CreateGBufferTexture(GL_TEXTURE1, GL_RGB32F, deferred_normal_texture);
    CreateGBufferTexture(GL_TEXTURE2, GL_RGB8, deferred_color_texture);
    CreateGBufferTexture(GL_TEXTURE3, GL_R16F, ao_textures[0]);
    CreateGBufferTexture(GL_TEXTURE3, GL_R16F, ao_textures[1]);

    glGenFramebuffers(1, &deferred_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer_obj);

    glGenRenderbuffers(1, &deferred_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, deferred_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, deferred_depth_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferred_position_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferred_normal_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferred_color_texture, 0);

    GLenum draw_buffers[]
    {
        GL_NONE,
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_NONE,
    };
    glDrawBuffers(5, draw_buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &ssao_frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_frame_buffer_obj);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ao_textures[0], 0);

    GLenum draw_buffers2[] = { GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(5, draw_buffers2);

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

void BuildKernel()
{
    int kernel_size = 64;
    std::vector<float> kernel(3 * kernel_size);
    for (int i = 0; i < kernel_size; ++i)
    {
        glm::vec3 random_direction = random.UniformHemisphere();
        float scale = static_cast<float>(i * i) / (kernel_size * kernel_size);
        random_direction *= glm::mix(0.1f, 1.0f, scale);

        kernel[i * 3 + 0] = random_direction.x;
        kernel[i * 3 + 1] = random_direction.y;
        kernel[i * 3 + 2] = random_direction.z;
    }

    GLuint program_handle = program.GetHandle();
    GLint location = glGetUniformLocation(program_handle, "u_sampler_kernel");
    glUniform3fv(location, kernel_size, kernel.data());
}

GLuint BuildRandomTexture()
{
    int size = 4;
    std::vector<GLfloat> random_directions(3 * size * size);
    for (int i = 0; i < size * size; ++i)
    {
        glm::vec3 v = random.UniformCircle();
        random_directions[i * 3 + 0] = v.x;
        random_directions[i * 3 + 1] = v.y;
        random_directions[i * 3 + 2] = v.z;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, size, size);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGB, GL_FLOAT, random_directions.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    return texture;
}

void Pass1()
{
    program.SetUniform("u_pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer_obj);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt
    (
        glm::vec3(2.1f, 1.5f, 2.1f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    projection = scene_projection;

    DrawScene();
}

void Pass2()
{
    program.SetUniform("u_pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, ssao_frame_buffer_obj);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ao_textures[0], 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    program.SetUniform("u_projection_matrix", scene_projection);

    DrawQuad();
}

void Pass3()
{
    program.SetUniform("u_pass", 3);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ao_textures[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ao_textures[1], 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    DrawQuad();
}

void Pass4()
{
    program.SetUniform("u_pass", 4);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ao_textures[1]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    DrawQuad();
}

void DrawScene()
{
    program.SetUniform("u_light.position_in_view", view * glm::vec4(3.0f, 3.0f, 1.5f, 1.0f));

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, wood_texture);
    program.SetUniform("u_material.is_use_texture", 1);
    model = glm::mat4(1.0f);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, brick_texture);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    plane->Render();

    program.SetUniform("u_material.is_use_texture", 0);
    program.SetUniform("u_material.Kd", glm::vec3(0.9f, 0.5f, 0.2f));
    model = glm::rotate(glm::mat4(1.0f), glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.282958, 0.0f));
    mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);
    obj_mesh->Render();
}

void DrawQuad()
{
    view = glm::mat4(1.0);
    model = glm::mat4(1.0);
    projection = glm::mat4(1.0);
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}