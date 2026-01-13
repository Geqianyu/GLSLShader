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
GLuint fragment_shader = 0;
GLuint shader_program = 0;
float last_time = 0.0f;
float angle = 0.0f;

std::string LoadShaderSource(const std::filesystem::path shader_file_path);
std::string GetShaderInfoLog(GLuint shader);
std::string GetProgramInfoLog(GLuint program);

void InitVertexData();
void LoadShaderFromSourceCode();
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

    window = glfwCreateWindow(800, 600, "Chapter2", nullptr, nullptr);
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

    // 初始化几何数据
    InitVertexData();

    // 从着色器源代码加载和编译着色器
    LoadShaderFromSourceCode();

    last_time = static_cast<float>(glfwGetTime());

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        Update();

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    glDeleteProgram(shader_program);
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
    std::string vertex_shader_source = LoadShaderSource("../../assets/shaders/chapter02/use_uniform.vs.glsl");
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
    std::string fragment_shader_source = LoadShaderSource("../../assets/shaders/chapter02/use_uniform.fs.glsl");
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

    // 使用着色器程序
    glUseProgram(shader_program);

    // 输出 属性 信息
    GLint attribs_num = 0;
    glGetProgramInterfaceiv(shader_program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &attribs_num);
    GLenum attribs_properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    std::cout << "Active attributes:" << std::endl;
    for (int i = 0; i < attribs_num; ++i)
    {
        GLint results[3] = { 0, 0, 0 };
        glGetProgramResourceiv(shader_program, GL_PROGRAM_INPUT, i, 3, attribs_properties, 3, nullptr, results);

        GLint name_buffer_size = results[0] + 1;
        char* name = new char[name_buffer_size];
        glGetProgramResourceName(shader_program, GL_PROGRAM_INPUT, i, name_buffer_size, nullptr, name);
        std::cout << "    location: " << results[2] << ", name: " << name << ", type: " << results[1] << std::endl;
        delete[] name;
    }

    // 输出 uniform 变量信息
    GLint uniforms_num = 0;
    glGetProgramInterfaceiv(shader_program, GL_UNIFORM,GL_ACTIVE_RESOURCES, &uniforms_num);
    GLenum uniforms_properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    std::cout << "Active uniforms:" << std::endl;
    for (int i = 0; i < uniforms_num; ++i)
    {
        GLint results[4] = { 0, 0, 0 };
        glGetProgramResourceiv(shader_program, GL_UNIFORM, i, 3, uniforms_properties, 3, nullptr, results);
        GLint name_buffer_size = results[0] + 1;
        char* name = new char[name_buffer_size];
        glGetProgramResourceName(shader_program, GL_UNIFORM, i, name_buffer_size, nullptr, name);
        std::cout << "    location: " << results[2] << ", name: " << name << ", type: " << results[1] << std::endl;
        delete[] name;
    }
}

void Update()
{
    // 更新旋转角度
    float current_time = static_cast<float>(glfwGetTime());
    float delta_time = current_time - last_time;
    last_time = current_time;
    angle += 180.0f * delta_time;
    if (angle > 360.0f)
    {
        angle -= 360.0f;
    }

    // 计算变换矩阵
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

    // 更新着色器中的 uniform
    GLint location = glGetUniformLocation(shader_program, "u_model");
    if (location >= 0)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rotation_matrix));
    }
}