# Chapter2 通过 uniform 传递数据

[返回](../../README.md)

本章介绍通过 uniform 将数据传递给着色器程序的基本方法。

## 2.1 定义 uniform 变量

在着色器程序中用 `uniform` 关键字定义 uniform 变量:

``` GLSL
uniform mat4 model = mat4(1.0);
```

uniform 变量在程序运行时可以被 CPU 端代码修改，但是不能在着色器程序中被修改，可以通过在着色器程序中紧跟在声明后面赋值给 uniform 变量赋初始值。

## 2.2 获取 uniform 变量位置

通过 `glGetUniformLocation` 获取 uniform 变量的位置:

``` GLSL
GLint location = glGetUniformLocation(shader_program, "model");
```
`glGetUniformLocation` 函数的一个参数是着色器程序对象，第二个参数是 uniform 变量的名称，返回值是 uniform 变量的位置，如果该变量不在着色器程序中，则返回 -1。

## 2.3 设置 uniform 变量值

通过 `glUniform*` 函数设置 uniform 变量的值:

``` C++
glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rotation_matrix));
```

本例子中，使用 `glUniformMatrix4fv` 函数将一个 4x4 矩阵传递给 uniform 变量。

## 2.4 输出 uniform 变量信息

``` C++
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
```

[返回](../../README.md)