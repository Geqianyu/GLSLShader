# Chapter03 使用 uniform block 和 uniform buffer object

[返回](../../README.md)

本章介绍通过 uniform block 和 uniform buffer object 将数据传递给着色器程序的基本方法。

## 3.1 在着色器程序中定义 uniform block

``` GLSL
uniform BlobSetting
{
    vec4 u_inner_color;
    vec4 u_outer_color;
    float u_inner_radius;
    float u_outer_radius;
};
```

如上述代码所示，这样可以定义一个 uniform block。

## 3.2 在 CPU 端代码中获取 uniform block 索引

通过 `glGetUniformBlockIndex` 获取 uniform block 索引:

``` C++
GLuint uniform_block_index = glGetUniformBlockIndex(shader_program, "BlobSetting");
```

## 3.3 在 CPU 端为 uniform block 分配内存空间

通过 `glGetActiveUniformBlockiv` 获取 uniform block 大小，并为其分配内存空间:

``` C++
GLint uniform_block_size = 0;
glGetActiveUniformBlockiv(shader_program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniform_block_size);
uniform_block_buffer = new GLubyte[uniform_block_size];
```

## 3.4 查询 uniform 变量在 uniform block 中的偏移

先通过 `glGetUniformIndices` 获取 uniform 变量的索引，然后通过 `glGetActiveUniformsiv` 获取偏移:

``` C++
const GLchar* uniform_names[] = { "u_inner_color", "u_outer_color", "u_inner_radius", "u_outer_radius" };
GLuint unifrom_indices[4] = { 0, 0, 0, 0 };
GLint uniform_offsets[] = { 0, 0, 0, 0 };
glGetUniformIndices(shader_program, 4, uniform_names, unifrom_indices);
glGetActiveUniformsiv(shader_program, 4, unifrom_indices, GL_UNIFORM_OFFSET, uniform_offsets);
```

## 3.5 将数据拷贝到内存中

C++ 中可以使用 `memcpy_s` 拷贝数据:

``` C++
GLfloat inner_color[] = { 1.0f, 1.0f, 0.75f, 1.0f };
GLfloat outer_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat inner_raidus = 0.25f;
GLfloat outer_radius = 0.45f;
memcpy_s(uniform_block_buffer + uniform_offsets[0], 4 * sizeof(float), inner_color, 4 * sizeof(float));
memcpy_s(uniform_block_buffer + uniform_offsets[1], 4 * sizeof(float), outer_color, 4 * sizeof(float));
memcpy_s(uniform_block_buffer + uniform_offsets[2], sizeof(float), &inner_raidus, sizeof(float));
memcpy_s(uniform_block_buffer + uniform_offsets[3], sizeof(float), &outer_radius, sizeof(float));
```

## 3.6 创建 uniform buffer object

通过 `glGenBuffers` 创建 uniform buffer object:

``` C++
glGenBuffers(1, &ubo);
```

uniform buffer 也是一块缓冲区，与 vbo 一样都使用 `glGenBuffers` 创建。用 `glBindBuffer` 绑定缓冲区，用 `glBufferData` 传输数据:

``` C++
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferData(GL_UNIFORM_BUFFER, uniform_block_size, uniform_block_buffer, GL_DYNAMIC_DRAW);
```

## 3.7 将 uniform block buffer 绑定到对应的 binding point

通过 `glBindBufferBase` 将 uniform block 绑定到对应的 binding point:

``` C++
glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
```

[返回](../../README.md)