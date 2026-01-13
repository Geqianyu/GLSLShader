# Chapter04 使用 uniform block 实例 和 uniform buffer object

[返回](../../README.md)

本章介绍通过 uniform block 实例 和 uniform buffer object 将数据传递给着色器程序的基本方法。

## 4.1 在 着色器程序 中定义 uniform block 实例

``` GLSL
uniform BlobSetting
{
    vec4 inner_color;
    vec4 outer_color;
    float inner_radius;
    float outer_radius;
} u_Bolb;
```

## 4.2 使用 uniform block 实例 中的变量

``` GLSL
fragment_color = mix(u_Bolb.inner_color, u_Bolb.outer_color, smoothstep(u_Bolb.inner_radius, u_Bolb.outer_radius, dist));
```

与非实例的不同是需要在变量名前加上实例名 `u_Bolb.`。

## 4.3 在 CPU 端代码查询 uniform 变量

与非实例的 uniform block 查询方法相同，不同点在于变量名前需要加上 uniform block 的名称 `BlobSetting.`:

``` C++
const GLchar* uniform_names[] = { "BlobSetting.inner_color", "BlobSetting.outer_color", "BlobSetting.inner_radius", "BlobSetting.outer_radius" };
GLuint unifrom_indices[4] = { 0, 0, 0, 0 };
glGetUniformIndices(shader_program, 4, uniform_names, unifrom_indices);
glGetActiveUniformsiv(shader_program, 4, unifrom_indices, GL_UNIFORM_OFFSET, uniform_offsets);
```

非实例的内容查看 [Chapter03 使用 uniform block 和 uniform buffer object](../chapter03/Chapter03.md)

[返回](../../README.md)