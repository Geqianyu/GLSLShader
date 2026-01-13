#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

GLFWwindow* window = nullptr;
GLuint vertex_shader = 0;
GLuint fragment_shader = 0;
GLuint shader_program = 0;
GLenum shader_format = 0;

std::string LoadShaderSource(const std::filesystem::path shader_file_path);
std::string GetShaderInfoLog(GLuint shader);
std::string GetProgramInfoLog(GLuint program);

void LoadShaderFromSourceCode();
void SaveShaderBinary();
void LoadShaderFromBinary();

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

    window = glfwCreateWindow(800, 600, "Chapter00", nullptr, nullptr);
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

    // 设置背景颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    // 将着色器程序保存为二进制
    SaveShaderBinary();

    // 删除着色器程序
    glDeleteProgram(shader_program);
    shader_program = 0;

    // 从二进制加载着色器程序
    LoadShaderFromBinary();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    glDeleteProgram(shader_program);
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
    std::string vertex_shader_source = LoadShaderSource("../../assets/shaders/chapter00/basic.vs.glsl");
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
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragment_shader == 0)
    {
        std::cerr << "创建 片元着色器 失败" << std::endl;
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::string fragment_shader_source = LoadShaderSource("../../assets/shaders/chapter00/basic.fs.glsl");
    const GLchar* fragment_shader_source_array[] = { fragment_shader_source.c_str() };
    glShaderSource(fragment_shader, 1, fragment_shader_source_array, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "编译 片元着色器 失败" << std::endl;
        std::cerr << GetShaderInfoLog(fragment_shader) << std::endl;
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 链接着色器程序
    shader_program = glCreateProgram();
    if (shader_program == 0)
    {
        std::cerr << "创建 着色器程序 失败" << std::endl;
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cerr << "链接 着色器程序 失败" << std::endl;
        std::cerr << GetProgramInfoLog(shader_program) << std::endl;
        glDeleteProgram(shader_program);
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 清除着色器对象
    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void SaveShaderBinary()
{
    GLint format = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &format);
    if (format < 1)
    {
        std::cerr << "不支持二进制形式的着色器程序" << std::endl;
        glDeleteProgram(shader_program);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    GLint length = 0;
    glGetProgramiv(shader_program, GL_PROGRAM_BINARY_LENGTH, &length);

    std::vector<GLubyte> buffer(length);
    glGetProgramBinary(shader_program, length, nullptr, &shader_format, buffer.data());

    std::string binary_shader_file_path("../../assets/shaders/chapter00/basic.bin.glsl");
    std::cout << "保存二进制着色器程序: " << binary_shader_file_path << ", 格式: " << shader_format << std::endl;

    std::ofstream shader_file(binary_shader_file_path, std::ios::binary);
    shader_file.write(reinterpret_cast<char*>(buffer.data()), length);
    shader_file.close();
}

void LoadShaderFromBinary()
{
    shader_program = glCreateProgram();
    if (shader_program == 0)
    {
        std::cerr << "创建 二进制 着色器程序 失败" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::ifstream binary_shader_file("../../assets/shaders/chapter00/basic.bin.glsl", std::ios::binary);
    std::istreambuf_iterator<char> start_it(binary_shader_file);
    std::istreambuf_iterator<char> end_it;
    std::vector<char> buffer(start_it, end_it);
    binary_shader_file.close();
    glProgramBinary(shader_program, shader_format, buffer.data(), buffer.size());

    GLint status = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        std::cerr << "链接 二进制 着色器程序 失败" << std::endl;
        std::cerr << GetProgramInfoLog(shader_program) << std::endl;
        glDeleteProgram(shader_program);
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}