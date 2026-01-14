# Chapter05 使用程序管线

[返回](../../README.md)

本章介绍着色器程序管线的使用方法。

程序管线对象作为可分离着色器对象扩展的一部分被引入，并在 OpenGL 4.1 版本中移入核心功能。
它们允许程序员从多个可分离着色器程序中混合和匹配着色器阶段。

假设有一个顶点着色器 **A**，两个片元着色器 **B** 和 **C**，**A** 配合 **B** 或者 **A** 配合 **C** 都可以正常工作。
可以创建两个着色器程序 **D** 和 **E**，其中 **D** 包含 **A** 和 **B**，**E** 包含 **A** 和 **C**。这样切换两个着色器程序也可以达到想要实现的渲染效果。
但是，如果顶点着色器 **A** 中包含多个 **uniform** 变量，那么每次切换着色器程序时都需要重新设置一遍 **uniform** 变量。
因为 **uniform** 变量是**着色器程序状态**的一部分，因此一个着色器程序中的 **uniform** 变量的修改不会传递到另一个着色器程序中。

**注**：**uniform** 变量保存在**着色器程序对象**中，而不是保存在**着色器对象**中。

通过可分离着色器对象，可以创建包含一个或多个着色器阶段的着色器程序。

通过程序管线，可以创建将可分离程序的各个阶段混合搭配的管线。
通过这种方式可以避免切换着色器阶段时丢失其他着色器阶段中 **uniform** 变量的值。
同样针对有一个顶点着色器 **A**，两个片元着色器 **B** 和 **C**，**A** 配合 **B** 或者 **A** 配合 **C** 都可以正常工作的情况。
可以创建 **3** 个**着色器程序**，每个**着色器程序**包含单个**着色器阶段**，分别是 **a**、**b** 和 **c**，然后创建 **2** 条**程序管线**，
第一条程序管线使用 **着色器程序a** 和 **着色器程序b**，第二条程序管线使用 **着色器程序a** 和 **着色器程序c**。
这样在两个**程序管线**间进行切换时，就不会丢失**顶点着色器阶段**中的 **uniform** 变量的值。

## 5.1 重新声明 gl_PerVertex

在可分离着色器中，如果使用了内置的 **gl_PerVertex** 输出块中的成员，就需要重新声明该输出块。
由于几乎总会使用到 **gl_Position**，程序，所以基本都需要重新声明:

``` GLSL
out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};
```

GLSL 的内置变量可以查看 [官方文档](https://wikis.khronos.org/opengl/Built-in_Variable_(GLSL))

## 5.2 创建一个顶点着色器和两个片元着色器

separable.vs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

uniform vec3 u_color_mask = vec3(0.0);

layout (location = 0) out vec3 color;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

void main()
{
    color = vertex_color * u_color_mask;
    gl_Position = vec4(vertex_position, 1.0);
}
```

separable1.fs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 fragment_color;

void main()
{
    fragment_color = vec4(color, 1.0);
}
```

separable2.fs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 fragment_color;

const float PI = 3.141592653589793;

void main()
{
    float fac = 1.0 + 0.5 * cos(gl_FragCoord.x * 100.0 / PI);
    fragment_color = vec4(fac * color, 1.0);
}
```

## 5.3 将着色器程序设置为可分离

通过 `glProgramParameteri` 设置着色器程序的参数。

``` C++
glProgramParameteri(shader_program_vertex, GL_PROGRAM_SEPARABLE, GL_TRUE);
glProgramParameteri(shader_program_fragment1, GL_PROGRAM_SEPARABLE, GL_TRUE);
glProgramParameteri(shader_program_fragment2, GL_PROGRAM_SEPARABLE, GL_TRUE);
```

通过设置之后，就可以将着色器程序设置为只有一个着色器阶段。

``` C++
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
```

用同样的方式可以链接着色器程序 **shader_program_fragment1** 和 **shader_program_fragment2**。

## 5.4 创建程序管线

通过 `glCreateProgramPipelines` 创建程序管线

``` C++
glCreateProgramPipelines(2, program_pipeline);
```

## 5.5 为程序管线绑定阶段

通过 `glUseProgramStages` 可以绑定阶段

``` C++
glUseProgramStages(program_pipeline[0], GL_VERTEX_SHADER_BIT, shader_program_vertex);
glUseProgramStages(program_pipeline[0], GL_FRAGMENT_SHADER_BIT, shader_program_fragment1);

glUseProgramStages(program_pipeline[1], GL_VERTEX_SHADER_BIT, shader_program_vertex);
glUseProgramStages(program_pipeline[1], GL_FRAGMENT_SHADER_BIT, shader_program_fragment2);
```

## 5.6 绘制

在绘制过程中，如果要设置 uniform 变量，只需要针对对应的**着色器程序**设置即可，在本例子中，需要对 **shader_program_vertex** 设置 **uniform** 变量 **u_color_mask**。

``` C++
GLint location = glGetUniformLocation(shader_program_vertex, "u_color_mask");
glProgramUniform3f(shader_program_vertex, location, 0.0f, 1.0f, 0.0f);
```

然后对不同的绘制策略选择不同的程序管线即可，使用 `glBindProgramPipeline` 可以选择使用的程序管线。

``` C++
glViewport(0, 0, 400, 600);
glBindProgramPipeline(program_pipeline[0]);
glDrawArrays(GL_TRIANGLES, 0, 3);

glViewport(400, 0, 400, 600);
glBindProgramPipeline(program_pipeline[1]);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

这样在两次绘制中都会使用 **shader_program_vertex** 中的 **uniform** 变量，不需要重新设置。

[返回](../../README.md)