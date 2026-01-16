# Chapter11 平面渲染

[返回](../../README.md)

顶点着色涉及在每个顶点计算着色模型，并将结果与该顶点关联。
然后，这些颜色会在多边形的面上进行插值，以产生平滑的着色效果。
这也被称为**高洛德(Gouraud)**着色。
在早期版本的 OpenGL 中，这种带颜色插值的顶点着色是默认的着色技术。

有时为每个多边形使用单一颜色，这样多边形的表面就不会出现颜色变化，使每个多边形看起来平坦。
这种方法可能会很有用，例如，希望有些表面看起来是平的，或者帮助可视化复杂网格中多边形的位置。
为每个多边形使用单一颜色称为**平面着色**。

在早期版本的 OpenGL 中，可以通过调用函数 `glShadeModel` 并传入参数 `GL_FLAT` 来启用平面着色。
在 OpenGL 4 中，**平面着色**通过着色器输入/输出变量可用的插值限定符 `flat` 来实现。

## 11.1 实现着色器

flat.vs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) flat out vec3 light_intensity;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    vec3 normal = normalize( u_normal_matrix * vertex_normal);
    vec4 position_in_view = u_view_model_matrix * vec4(vertex_position,1.0);
    
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(vec3(u_light.position_in_view - position_in_view));
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_light.Ld * u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position_in_view.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_light.Ls * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }

    light_intensity = ambient_color + diffuse_color + specular_color;

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}
```

flat.fs.glsl
``` GLSL
#version 460

layout (location = 0) flat in vec3 light_intensity;

layout (location = 0) out vec4 fragment_color;

void main()
{
    fragment_color = vec4(light_intensity, 1.0);
}
```

## 11.2 配置触发顶点

``` C++
glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
```

通过使用 **flat** 限定符修饰顶点输出变量（及其对应的片段输入变量）可以启用平面着色。
该限定符表示在值到达片段着色器之前不会进行插值。呈现给片段着色器的值将对应于多边形第一个或最后一个顶点的顶点着色器调用的结果。
该顶点称为触发顶点，可以使用 OpenGL 函数 `glProvokingVertex` 进行配置。
`GL_LAST_VERTEX_CONVENTION` 参数表示应使用最后一个顶点。默认值是 `GL_LAST_VERTEX_CONVENTION`。

[返回](../../README.md)