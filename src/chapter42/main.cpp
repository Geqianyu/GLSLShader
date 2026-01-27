#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/cube.h"
#include "common/sphere.h"

#include <iostream>
#include <memory>

struct ListNode
{
    glm::vec4 color;
    GLfloat depth;
    GLuint next;
};

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Cube> cube;
std::unique_ptr<glsl_shader::Sphere> sphere;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint quad_vao = 0;
GLuint quad_vertices = 0;
GLuint buffers[2] = { 0, 0 };
GLuint head_ptr_texture = 0;
GLuint clear_buffer = 0;
GLuint pass1_index = 0;
GLuint pass2_index = 0;
const int COUNTER_BUFFER = 0;
const int LINKED_LIST_BUFFER = 1;
float angle = glm::radians(210.0f);

void LoadShaderFromSourceCode();
void CreateGBufferTexture(GLenum texture_unit, GLenum format, GLuint& texture);
void InitShaderStorage();
void TerminateShaderStorage();
void InitGeometry();
void TerminateGeometry();
void Pass1();
void Pass2();
void ClearBuffers();
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

    window = glfwCreateWindow(800, 600, "Chapter42", nullptr, nullptr);
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

    // 启动混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化着色器存储
    InitShaderStorage();

    GLuint program_handle = program.GetHandle();
    pass1_index = glGetSubroutineIndex(program_handle, GL_FRAGMENT_SHADER, "Pass1");
    pass2_index = glGetSubroutineIndex(program_handle, GL_FRAGMENT_SHADER, "Pass2");

    // 初始化几何体
    InitGeometry();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        ClearBuffers();
        Pass1();
        glFlush();
        Pass2();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    TerminateGeometry();
    TerminateShaderStorage();
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    program.CompileShader("../../assets/shaders/chapter42/oit.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter42/oit.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();
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

void InitShaderStorage()
{
    glGenBuffers(2, buffers);
    GLuint max_nodes = 20 * 800 * 600;
    GLint node_size = sizeof(ListNode);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

    glGenTextures(1, &head_ptr_texture);
    glBindTexture(GL_TEXTURE_2D, head_ptr_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 800, 600);
    glBindImageTexture(0, head_ptr_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[LINKED_LIST_BUFFER]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_nodes * node_size, nullptr, GL_DYNAMIC_DRAW);

    program.SetUniform("u_max_nodes", max_nodes);

    std::vector<GLuint> head_ptr_clear_buffer(800 * 600, 0xffffffff);
    glGenBuffers(1, &clear_buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_buffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, head_ptr_clear_buffer.size() * sizeof(GLuint), &head_ptr_clear_buffer[0], GL_STATIC_COPY);
}

void TerminateShaderStorage()
{
    glDeleteBuffers(2, buffers);
    glDeleteTextures(1, &head_ptr_texture);
    glDeleteBuffers(1, &clear_buffer);
}

void InitGeometry()
{
    cube = std::make_unique<glsl_shader::Cube>();
    sphere = std::make_unique<glsl_shader::Sphere>(1.0f, 40, 40);

    GLfloat vertices[]
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quad_vertices);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void TerminateGeometry()
{
    cube.release();
    sphere.release();
    glDeleteBuffers(1, &quad_vertices);
    glDeleteVertexArrays(1, &quad_vao);
}

void Pass1()
{
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1_index);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(glm::vec3(11.0f * glm::cos(angle), 2.0f, 11.0f * glm::sin(angle)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 1.0f, 1000.0f);

    glDepthMask(GL_FALSE);

    DrawScene();

    glFinish();
}

void Pass2()
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2_index);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    glm::mat4 mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ClearBuffers()
{
    GLuint zero = 0;
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_buffer);
    glBindTexture(GL_TEXTURE_2D, head_ptr_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
}

void DrawScene()
{
    program.SetUniform("u_light_position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    program.SetUniform("u_light_intensity", glm::vec3(0.9f));
    program.SetUniform("u_Kd", glm::vec4(0.2f, 0.2f, 0.9f, 0.55f));

    float size = 0.45f;
    for (int i = 0; i <= 6; ++i)
    {
        for (int j = 0; j <= 6; ++j)
        {
            for (int k = 0; k <= 6; ++k)
            {
                if ((i + j + k) % 2 == 0)
                {
                    model = glm::translate(glm::mat4(1.0f), glm::vec3(i - 3.0f, j - 3.0f, k - 3.0f));
                    model = glm::scale(model, glm::vec3(size));
                    glm::mat4 mv = view * model;
                    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
                    program.SetUniform("u_view_model_matrix", mv);
                    program.SetUniform("u_mvp_matrix", projection * mv);
                    cube->Render();
                }
            }
        }
    }

    program.SetUniform("u_Kd", glm::vec4(0.9f, 0.2f, 0.2f, 0.4f));
    size = 2.0f;
    float position = 1.75f;
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-position, -position, position));
    model = glm::scale(model, glm::vec3(size));
    glm::mat4 mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-position, -position, -position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-position, position, position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-position, position, -position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(position, position, position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(position, position, -position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(position, -position, position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(position, -position, -position));
    model = glm::scale(model, glm::vec3(size));
    mv = view * model;
    program.SetUniform("u_normal_matrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_mvp_matrix", projection * mv);
    cube->Render();
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