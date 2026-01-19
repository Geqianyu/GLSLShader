#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/plane.h"
#include "common/obj_mesh.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram program;
std::unique_ptr<glsl_shader::Plane> plane;
std::unique_ptr<glsl_shader::ObjMesh> obj_mesh;
float angle = 0.0f;
float last_time = 0.0f;
glm::vec4 light_position = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 4.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.5f, 100.0f);

void LoadShaderFromSourceCode();
void InitGeometry();
void TerminateGeometry();
void DrawCow(const glm::vec3& position, float roughness, int is_metal, const glm::vec3& color);

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

    window = glfwCreateWindow(800, 600, "Chapter21", nullptr, nullptr);
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

    // 设置平面着色时使用第一个顶点的颜色
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化几何体
    InitGeometry();

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float current_time = static_cast<float>(glfwGetTime());
        angle += 1.5f * (current_time - last_time);
        last_time = current_time;
        if (angle > glm::two_pi<float>())
        {
            angle -= glm::two_pi<float>();
        }

        light_position.x = glm::cos(angle) * 7.0f;
        light_position.y = 3.0f;
        light_position.z = glm::sin(angle) * 7.0f;

        program.SetUniform("u_lights[0].position_in_view", view * light_position);

        // 绘制地板
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
        glm::mat4 mv = view * model;
        program.SetUniform("u_view_model_matrix", mv);
        program.SetUniform("u_normal_matrix", glm::mat3(mv));
        program.SetUniform("u_mvp_matrix", projection * mv);
        program.SetUniform("u_material.roughness", 0.9f);
        program.SetUniform("u_material.is_metal", 0);
        program.SetUniform("u_material.color", glm::vec3(0.2f));
        plane->Render();

        // 绘制 非金属 牛
        int num_cows = 9;
        glm::vec3 cow_base_color(0.1f, 0.33f, 0.17f);
        for (int i = 0; i < num_cows; ++i)
        {
            float cow_x = i * (10.0f / (num_cows - 1)) - 5.0f;
            float roughness = (i + 1) * (1.0f / num_cows);
            DrawCow(glm::vec3(cow_x, 0.0f, 0.0f), roughness, 0, cow_base_color);
        }

        // 绘制 金属 牛
        float metal_roughness = 0.43f;
        // Gold
        DrawCow(glm::vec3(-3.0f, 0.0f, 3.0f), metal_roughness, 1, glm::vec3(1, 0.71f, 0.29f));
        // Copper
        DrawCow(glm::vec3(-1.5f, 0.0f, 3.0f), metal_roughness, 1, glm::vec3(0.95f, 0.64f, 0.54f));
        // Aluminum
        DrawCow(glm::vec3(-0.0f, 0.0f, 3.0f), metal_roughness, 1, glm::vec3(0.91f, 0.92f, 0.92f));
        // Titanium
        DrawCow(glm::vec3(1.5f, 0.0f, 3.0f), metal_roughness, 1, glm::vec3(0.542f, 0.497f, 0.449f));
        // Silver
        DrawCow(glm::vec3(3.0f, 0.0f, 3.0f), metal_roughness, 1, glm::vec3(0.95f, 0.93f, 0.88f));

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
    program.CompileShader("../../assets/shaders/chapter21/pbr.vs.glsl");
    program.CompileShader("../../assets/shaders/chapter21/pbr.fs.glsl");
    program.Link();
    program.Use();
    program.PrintActiveAttribs();
    program.PrintActiveUniformBlocks();
    program.PrintActiveUniforms();

    program.SetUniform("u_lights[0].L", glm::vec3(45.0f));
    program.SetUniform("u_lights[0].position_in_view", view * light_position);
    program.SetUniform("u_lights[1].L", glm::vec3(0.3f));
    program.SetUniform("u_lights[1].position_in_view", glm::vec4(0.0f, 0.15f, -1.0f, 0.0f));
    program.SetUniform("u_lights[2].L", glm::vec3(45.0f));
    program.SetUniform("u_lights[2].position_in_view", view * glm::vec4(-7.0f, 3.0f, 7.0f, 1.0f));
}

void InitGeometry()
{
    plane = std::make_unique<glsl_shader::Plane>(20.0f, 20.0f, 1, 1);
    obj_mesh = glsl_shader::ObjMesh::Load("../../assets/models/spot_triangulated.obj");
}

void TerminateGeometry()
{
    plane.release();
    obj_mesh.release();
}

void DrawCow(const glm::vec3& position, float roughness, int is_metal, const glm::vec3& color)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 mv = view * model;
    program.SetUniform("u_view_model_matrix", mv);
    program.SetUniform("u_normal_matrix", glm::mat3(mv));
    program.SetUniform("u_mvp_matrix", projection * mv);

    program.SetUniform("u_material.roughness", roughness);
    program.SetUniform("u_material.is_metal", is_metal);
    program.SetUniform("u_material.color", color);

    obj_mesh->Render();
}