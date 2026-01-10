# Chapter0 着色器的加载、编译和保存

[返回](../../README.md)

本章主要介绍着色器的加载和保存

## 0.1 输出信息

``` C++
const GLubyte* renderer = glGetString(GL_RENDERER);                         // 显卡信息
const GLubyte* vendor = glGetString(GL_VENDOR);                             // 显卡厂商信息
const GLubyte* version = glGetString(GL_VERSION);                           // OpenGL 版本信息
const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);     // 着色器 版本信息
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
```

## 0.2 创建着色器

1. 通过 `glCreateShader` 创建着色器，着色器类型有以下集中：
    - `GL_VERTEX_SHADER`: 顶点着色器
    - `GL_FRAGMENT_SHADER`: 片段着色器
    - `GL_GEOMETRY_SHADER`: 几何着色器
    - `GL_TESS_CONTROL_SHADER`: 控制着色器
    - `GL_TESS_EVALUATION_SHADER`: 评估着色器
    - `GL_COMPUTE_SHADER`: 计算着色器

2. 通过 `glShaderSource` 将着色器源码传递给 OpenGL，案例如下:

``` C++
std::string vertex_shader_source = LoadShaderSource("../../assets/shaders/chapter0/basic.vs.glsl");
const GLchar* vertex_shader_source_array[] = { vertex_shader_source.c_str() };
glShaderSource(vertex_shader, 1, vertex_shader_source_array, nullptr);
```

3. 通过 `glCompileShader` 编译着色器，编译完成后使用 `glGetShaderiv` 检查编译状态，如果编译**不**成功则用 `glGetShaderInfoLog` 获取错误信息，案例如下:

``` C++
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
```

其中 `GetShaderInfoLog` 函数实现如下:

``` C++
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
```

4. 通过 `glCreateProgram` 创建程序对象，并通过 `glAttachShader` 将编译好的着色器附加到程序对象上，最后通过 `glLinkProgram` 链接程序对象，链接完成后使用 `glGetProgramiv` 检查链接状态，如果链接**不**成功则用 `glGetProgramInfoLog` 获取错误信息，案例如下:

``` C++
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
```

其中 `GetProgramInfoLog` 函数实现如下:

``` C++
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
```

## 0.3 保存着色器程序为二进制文件

1. 将编译好的着色器程序保存为二进制文件
`GL_NUM_PROGRAM_BINARY_FORMATS` 可以获取支持的二进制格式数量，通过 `glGetIntegerv` 获取支持的二进制格式列表，案例如下:

``` C++
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
```

枚举 `GL_PROGRAM_BINARY_LENGTH` 可以获取着色器程序二进制数据的长度，可以通过 `glGetProgramiv` 获取长度值，案例如下:

``` C++
GLint length = 0;
glGetProgramiv(shader_program, GL_PROGRAM_BINARY_LENGTH, &length);
```

`glGetProgramBinary` 可以获取着色器程序的二进制数据，获取到数据后可以保存到文件中，案例如下:

``` C++
GLenum shader_format = 0;
std::vector<GLubyte> buffer(length);
glGetProgramBinary(shader_program, length, nullptr, &shader_format, buffer.data());

std::string binary_shader_file_path("../../assets/shaders/chapter0/basic.bin.glsl");
std::cout << "保存二进制着色器程序: " << binary_shader_file_path << ", 格式: " << shader_format << std::endl;

std::ofstream shader_file(binary_shader_file_path, std::ios::binary);
shader_file.write(reinterpret_cast<char*>(buffer.data()), length);
shader_file.close();
```

其中 `length` 是上一步获取的二进制数据长度，`shader_format` 是获取到的着色器程序格式，在后续加载二进制着色器程序是会用到。

## 0.4 加载二进制着色器程序

在这个步骤中会用到上一个步骤中得到的 `shader_format` 变量。通过 `glProgramBinary` 可以加载二进制着色器程序，案例如下:

``` C++
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

    std::ifstream binary_shader_file("../../assets/shaders/chapter0/basic.bin.glsl", std::ios::binary);
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
```

[返回](../../README.md)