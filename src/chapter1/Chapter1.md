# Chapter1 设定属性索引，传递几何数据并渲染三角形

[返回](../../README.md)

本章主要介绍传递几何数据，在未设定属性索引的情况下通过 C++ 设定属性索引并将渲染一个三角形

## 1.1 初始化几何数据

创建顶点数据缓冲区和顶点数组对象：

- `glGenBuffers` 可以创建顶点数据缓冲区
-
- `glBindBuffer` 绑定顶点数据缓冲区
-
- `glBufferData` 传输顶点数据到缓冲区
-
- `glGenVertexArrays` 可以创建顶点数组对象
-
- `glBindVertexArray` 绑定顶点数组对象
-
- `glEnableVertexAttribArray` 启用顶点属性数组
-
- `glBindVertexBuffer` 绑定顶点缓冲区到指定的绑定点
-
- `glVertexAttribFormat` 定义顶点属性数据格式
-
- `glVertexAttribBinding` 将顶点属性与绑定点关联

``` C++
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
```

## 1.2 指定属性索引

- `glBindAttribLocation` 可以在链接程序对象之前指定属性索引
- `glBindFragDataLocation` 可以在链接程序对象之前指定片段着色器输出变量的位置

``` C++
glBindAttribLocation(shader_program, 0, "vertex_position");
glBindAttribLocation(shader_program, 1, "vertex_color");
glBindFragDataLocation(shader_program, 0, "fragment_color");
```

## 1.3 渲染三角形

- `glDrawArrays` 可以渲染几何图元
- `glBindVertexArray` 绑定顶点数组对象

``` C++
glBindVertexArray(vao);
glDrawArrays(GL_TRIANGLES, 0, 3);
glBindVertexArray(0);
```

[返回](../../README.md)