#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

GLFWwindow* window = nullptr;
GLuint vbo[2] = { 0, 0 };
GLuint vao = 0;
GLuint vertex_shader = 0;
GLuint fragment_shader1 = 0;
GLuint fragment_shader2 = 0;
GLuint shader_program_vertex = 0;
GLuint shader_program_fragment1 = 0;
GLuint shader_program_fragment2 = 0;
GLuint program_pipeline[2] = { 0, 0 };

std::string LoadShaderSource(const std::filesystem::path shader_file_path);
std::string GetShaderInfoLog(GLuint shader);
std::string GetProgramInfoLog(GLuint program);

void InitVertexData();
void LoadShaderFromSourceCode();
void InitProgramPipeline();

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

    window = glfwCreateWindow(800, 600, "Chapter05", nullptr, nullptr);
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 启动混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 初始化几何数据
    InitVertexData();

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 初始化程序管线
    InitProgramPipeline();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glUseProgram(0);

        GLint location = glGetUniformLocation(shader_program_vertex, "u_color_mask");
        glProgramUniform3f(shader_program_vertex, location, 0.0f, 1.0f, 0.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glViewport(0, 0, 400, 600);
        glBindProgramPipeline(program_pipeline[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glViewport(400, 0, 400, 600);
        glBindProgramPipeline(program_pipeline[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    glDeleteProgramPipelines(2, program_pipeline);
    glDeleteProgram(shader_program_vertex);
    glDeleteProgram(shader_program_fragment1);
    glDeleteProgram(shader_program_fragment2);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

std::string LoadShaderSource(const std::filesystem::path shader_file_path)
{
    std::ifstream shader_file(shader_file_path);
    if (!shader_file.is_open())
    {
        std::cerr << "无法打开着色器文件: " << shader_file_path << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::stringstream shader_source_stream;
    shader_source_stream << shader_file.rdbuf();
    shader_file.close();
    return shader_source_stream.str();
}

std::string GetShaderInfoLog(GLuint shader)
{
    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::string log(log_length, '\0');
        GLsizei written_length = 0;
        glGetShaderInfoLog(shader, log_length, &written_length, log.data());
        return log;
    }
    return std::string();
}

std::string GetProgramInfoLog(GLuint program)
{
    GLint log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::string log(log_length, '\0');
        GLsizei written_length = 0;
        glGetProgramInfoLog(program, log_length, &written_length, log.data());
        return log;
    }
    return std::string();
}

void InitVertexData()
{
    float position_data[]
    {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.0f,  0.8f, 0.0f,
    };
    float color_data[]
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    // 创建缓冲区
    glGenBuffers(2, vbo);
    GLuint position_vbo = vbo[0];
    GLuint color_vbo = vbo[1];

    // 绑定缓冲区并传输数据
    glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position_data), position_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_data), color_data, GL_STATIC_DRAW);

    // 创建 VAO
    glGenVertexArrays(1, &vao);

    // 绑定 VAO 并设置格式
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexBuffer(0, position_vbo, 0, 3 * sizeof(float));
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glBindVertexBuffer(1, color_vbo, 0, 3 * sizeof(float));
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    glBindVertexArray(0);
}

void LoadShaderFromSourceCode()
{
    // 创建顶点着色器
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    if (vertex_shader == 0)
    {
        std::cerr << "创建 顶点着色器 失败" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::string vertex_shader_source = LoadShaderSource("../../assets/shaders/chapter05/separable.vs.glsl");
    const GLchar* vertex_shader_source_array[] = { vertex_shader_source.c_str() };
    glShaderSource(vertex_shader, 1, vertex_shader_source_array, nullptr);
    glCompileShader(vertex_shader);
    GLint result = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "编译 顶点着色器 失败" << std::endl;
        std::cerr << GetShaderInfoLog(vertex_shader) << std::endl;
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 创建片元着色器
    fragment_shader1 = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragment_shader1 == 0)
    {
        std::cerr << "创建 片元着色器 失败" << std::endl;
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::string fragment_shader_source1 = LoadShaderSource("../../assets/shaders/chapter05/separable1.fs.glsl");
    const GLchar* fragment_shader_source_array1[] = { fragment_shader_source1.c_str() };
    glShaderSource(fragment_shader1, 1, fragment_shader_source_array1, nullptr);
    glCompileShader(fragment_shader1);
    glGetShaderiv(fragment_shader1, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "编译 片元着色器1 失败" << std::endl;
        std::cerr << GetShaderInfoLog(fragment_shader1) << std::endl;
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    fragment_shader2 = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragment_shader2 == 0)
    {
        std::cerr << "创建 片元着色器2 失败" << std::endl;
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::string fragment_shader_source2 = LoadShaderSource("../../assets/shaders/chapter05/separable2.fs.glsl");
    const GLchar* fragment_shader_source_array2[] = { fragment_shader_source2.c_str() };
    glShaderSource(fragment_shader2, 1, fragment_shader_source_array2, nullptr);
    glCompileShader(fragment_shader2);
    glGetShaderiv(fragment_shader2, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "编译 片元着色器2 失败" << std::endl;
        std::cerr << GetShaderInfoLog(fragment_shader2) << std::endl;
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 创建着色器程序
    shader_program_vertex = glCreateProgram();
    if (shader_program_vertex == 0)
    {
        std::cerr << "创建 顶点着色器程序 失败" << std::endl;
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    shader_program_fragment1 = glCreateProgram();
    if (shader_program_fragment1 == 0)
    {
        std::cerr << "创建 片元着色器程序1 失败" << std::endl;
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    shader_program_fragment2 = glCreateProgram();
    if (shader_program_fragment2 == 0)
    {
        std::cerr << "创建 片元着色器程序2 失败" << std::endl;
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 将着色器程序设置为可分离
    glProgramParameteri(shader_program_vertex, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glProgramParameteri(shader_program_fragment1, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glProgramParameteri(shader_program_fragment2, GL_PROGRAM_SEPARABLE, GL_TRUE);

    // 链接着色器程序
    glAttachShader(shader_program_vertex, vertex_shader);
    glLinkProgram(shader_program_vertex);
    glGetProgramiv(shader_program_vertex, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "链接 顶点着色器程序 失败" << std::endl;
        std::cerr << GetProgramInfoLog(shader_program_vertex) << std::endl;
        glDeleteProgram(shader_program_vertex);
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glAttachShader(shader_program_fragment1, fragment_shader1);
    glLinkProgram(shader_program_fragment1);
    glGetProgramiv(shader_program_fragment1, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "链接 片元着色器1程序 失败" << std::endl;
        std::cerr << GetProgramInfoLog(shader_program_fragment1) << std::endl;
        glDeleteProgram(shader_program_fragment1);
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glAttachShader(shader_program_fragment2, fragment_shader2);
    glLinkProgram(shader_program_fragment2);
    glGetProgramiv(shader_program_fragment2, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "链接 片元着色器2程序 失败" << std::endl;
        std::cerr << GetProgramInfoLog(shader_program_fragment2) << std::endl;
        glDeleteProgram(shader_program_fragment2);
        glDeleteProgram(shader_program_fragment1);
        glDeleteShader(fragment_shader2);
        glDeleteShader(fragment_shader1);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 清除着色器对象
    glDetachShader(shader_program_vertex, vertex_shader);
    glDetachShader(shader_program_fragment1, fragment_shader1);
    glDetachShader(shader_program_fragment2, fragment_shader2);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader1);
    glDeleteShader(fragment_shader2);
}

void InitProgramPipeline()
{
    // 创建程序管线
    glCreateProgramPipelines(2, program_pipeline);

    // 设置程序管线
    glUseProgramStages(program_pipeline[0], GL_VERTEX_SHADER_BIT, shader_program_vertex);
    glUseProgramStages(program_pipeline[0], GL_FRAGMENT_SHADER_BIT, shader_program_fragment1);

    glUseProgramStages(program_pipeline[1], GL_VERTEX_SHADER_BIT, shader_program_vertex);
    glUseProgramStages(program_pipeline[1], GL_FRAGMENT_SHADER_BIT, shader_program_fragment2);
}