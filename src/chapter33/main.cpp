#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/glsl_program.h"
#include "common/obj_mesh.h"
#include "common/sky_box.h"
#include "common/texture.h"

#include <iostream>
#include <memory>

GLFWwindow* window = nullptr;
glsl_shader::GLSLProgram sky_box_program;
glsl_shader::GLSLProgram obj_mesh_program;
std::unique_ptr<glsl_shader::SkyBox> sky_box;
std::unique_ptr<glsl_shader::ObjMesh> obj_mesh;
glm::vec3 camera_position = glm::vec3(0.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 0.3f, 100.0f);
GLuint diffuse_ibl = 0;
GLuint sky_box_texture = 0;
GLuint mesh_texture = 0;
float angle = glm::half_pi<float>();
float last_time = 0.0f;

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

    window = glfwCreateWindow(800, 600, "Chapter33", nullptr, nullptr);
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

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_ibl);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh_texture);
        obj_mesh_program.Use();
        obj_mesh_program.SetUniform("u_camera_position", camera_position);
        model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
        obj_mesh_program.SetUniform("u_model_matrix", model);
        obj_mesh_program.SetUniform("u_normal_matrix", glm::transpose(glm::inverse(glm::mat3(model))));
        obj_mesh_program.SetUniform("u_mvp_matrix", projection * view * model);
        obj_mesh->Render();

        model = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box_texture);
        sky_box_program.Use();
        sky_box_program.SetUniform("u_mvp_matrix", projection * view * model);
        sky_box->Render();

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
    sky_box_program.CompileShader("../../assets/shaders/chapter33/sky_box.vs.glsl");
    sky_box_program.CompileShader("../../assets/shaders/chapter33/sky_box.fs.glsl");
    sky_box_program.Link();
    sky_box_program.Use();
    sky_box_program.PrintActiveAttribs();
    sky_box_program.PrintActiveUniformBlocks();
    sky_box_program.PrintActiveUniforms();

    obj_mesh_program.CompileShader("../../assets/shaders/chapter33/diffuse_ibl.vs.glsl");
    obj_mesh_program.CompileShader("../../assets/shaders/chapter33/diffuse_ibl.fs.glsl");
    obj_mesh_program.Link();
    obj_mesh_program.Use();
    obj_mesh_program.PrintActiveAttribs();
    obj_mesh_program.PrintActiveUniformBlocks();
    obj_mesh_program.PrintActiveUniforms();
}

void InitGeometry()
{
    obj_mesh = glsl_shader::ObjMesh::Load("../../assets/models/spot_triangulated.obj");
    sky_box = std::make_unique<glsl_shader::SkyBox>();
}

void TerminateGeometry()
{
    obj_mesh.release();
    sky_box.release();
}

void InitTextures()
{
    diffuse_ibl = glsl_shader::Texture::LoadHdrCubeMap("../../assets/textures/grace-diffuse");
    sky_box_texture = glsl_shader::Texture::LoadHdrCubeMap("../../assets/textures/grace");
    mesh_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/spot_texture.png");
}

void TerminateTextures()
{
    glDeleteTextures(1, &diffuse_ibl);
    glDeleteTextures(1, &sky_box_texture);
    glDeleteTextures(1, &mesh_texture);
}

void Update()
{
    float current_time = static_cast<float>(glfwGetTime());
    float delta_time = current_time - last_time;
    last_time = current_time;
    angle = glm::mod(angle + 0.5f * delta_time, glm::two_pi<float>());
    camera_position = glm::vec3(glm::cos(angle) * 4.0f, 0.0f, glm::sin(angle) * 4.0f);
    view = glm::lookAt(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}