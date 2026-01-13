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
GLuint ubo = 0;
GLubyte* uniform_block_buffer = nullptr;
GLint uniform_offsets[] = { 0, 0, 0, 0 };

std::string LoadShaderSource(const std::filesystem::path shader_file_path);
std::string GetShaderInfoLog(GLuint shader);
std::string GetProgramInfoLog(GLuint program);

void InitVertexData();
void LoadShaderFromSourceCode();
void SetUniformBlock();

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

    window = glfwCreateWindow(800, 600, "Chapter04", nullptr, nullptr);
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

    // 设置 uniform block
    SetUniformBlock();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // 清理和退出
    delete[] uniform_block_buffer;
    uniform_block_buffer = nullptr;
    glDeleteBuffers(1, &ubo);
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
         0.8f,  0.8f, 0.0f,
        -0.8f, -0.8f, 0.0f,
         0.8f,  0.8f, 0.0f,
        -0.8f,  0.8f, 0.0f
    };
    float uv_data[]
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // 创建缓冲区
    glGenBuffers(2, vbo);
    GLuint position_vbo = vbo[0];
    GLuint uv_vbo = vbo[1];

    // 绑定缓冲区并传输数据
    glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position_data), position_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);

    // 创建 VAO
    glGenVertexArrays(1, &vao);

    // 绑定 VAO 并设置格式
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexBuffer(0, position_vbo, 0, 3 * sizeof(float));
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glBindVertexBuffer(1, uv_vbo, 0, 2 * sizeof(float));
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);
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
    std::string vertex_shader_source = LoadShaderSource("../../assets/shaders/chapter04/use_uniform_block_instance.vs.glsl");
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
    std::string fragment_shader_source = LoadShaderSource("../../assets/shaders/chapter04/use_uniform_block_instance.fs.glsl");
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
}

void SetUniformBlock()
{
    // 获取 uniform block 索引
    GLuint uniform_block_index = glGetUniformBlockIndex(shader_program, "BlobSetting");

    // 为 uniform block 分配内存空间
    GLint uniform_block_size = 0;
    glGetActiveUniformBlockiv(shader_program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block_size);
    uniform_block_buffer = new GLubyte[uniform_block_size];

    // 查询 uniform 变量在 uniform block 中的偏移量
    const GLchar* uniform_names[] = { "BlobSetting.inner_color", "BlobSetting.outer_color", "BlobSetting.inner_radius", "BlobSetting.outer_radius" };
    GLuint unifrom_indices[4] = { 0, 0, 0, 0 };
    glGetUniformIndices(shader_program, 4, uniform_names, unifrom_indices);
    glGetActiveUniformsiv(shader_program, 4, unifrom_indices, GL_UNIFORM_OFFSET, uniform_offsets);

    // 将数据拷贝靠内存空间
    GLfloat inner_color[] = { 1.0f, 1.0f, 0.75f, 1.0f };
    GLfloat outer_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    GLfloat inner_raidus = 0.25f;
    GLfloat outer_radius = 0.45f;
    memcpy_s(uniform_block_buffer + uniform_offsets[0], 4 * sizeof(float), inner_color, 4 * sizeof(float));
    memcpy_s(uniform_block_buffer + uniform_offsets[1], 4 * sizeof(float), outer_color, 4 * sizeof(float));
    memcpy_s(uniform_block_buffer + uniform_offsets[2], sizeof(float), &inner_raidus, sizeof(float));
    memcpy_s(uniform_block_buffer + uniform_offsets[3], sizeof(float), &outer_radius, sizeof(float));

    // 创建 uniform block buffer
    glGenBuffers(1, &ubo);

    // 将数据上传
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, uniform_block_size, uniform_block_buffer, GL_DYNAMIC_DRAW);

    // 将 uniform block buffer 绑定到对应的 binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    // 输出 uniform 变量信息
    GLint uniforms_num = 0;
    glGetProgramInterfaceiv(shader_program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniforms_num);
    GLenum uniforms_properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    std::cout << "Active uniforms:" << std::endl;
    for (int i = 0; i < uniforms_num; ++i)
    {
        GLint results[3] = { 0, 0, 0 };
        glGetProgramResourceiv(shader_program, GL_UNIFORM, i, 3, uniforms_properties, 3, nullptr, results);
        GLint name_buffer_size = results[0] + 1;
        char* name = new char[name_buffer_size];
        glGetProgramResourceName(shader_program, GL_UNIFORM, i, name_buffer_size, nullptr, name);
        std::cout << "    location: " << results[2] << ", name: " << name << ", type: " << results[1] << std::endl;
        delete[] name;
    }
}