# Chapter16 使用片元着色器进行光照计算

[返回](../../README.md)

当在顶点着色器中计算着色。然后在片元着色器中进行插值，该插值颜色分配给输出片段。这种技术称为古罗照明(**Gouraud** 着色)。

**Gouraud** 着色是一种近似方法，当顶点处的反射特性与多边形中心的特性差异较大时，可能会导致一些不理想的结果。
例如，一个明亮的高光可能出现在多边形的中心，但不在其顶点。
如果只是对顶点处的着色方程进行评估，高光就不会出现在渲染结果中。
当使用 **Gouraud** 着色时，还可能出现其他不理想的伪影，
如多边形的边缘，这是因为颜色插值可能无法与面上的反射模型值匹配。

为了提高结果的精确度，可以将着色方程的计算从顶点着色器移动到片元着色器。
不在多边形上插值颜色，而是插值位置和法向量，并使用这些值在每个片元上计算着色方程。
这种技术称为**Phong**着色或**Phong**插值。
**Phong**着色的结果更加精确，效果也更令人满意，但仍可能出现一些不理想的伪影。

## 16.1 将光照计算从顶点着色器移到片元着色器中

per_fragment.vs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 position_in_view;
layout (location = 1) out vec3 normal_in_view;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    position_in_view = (u_view_model_matrix * vec4(vertex_position, 1.0)).xyz;
    normal_in_view = u_normal_matrix * vertex_normal;

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}
```

per_fragment.fs.glsl
``` GLSL
#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 La;
    vec3 L;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

vec3 CalculatePhongModel(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(u_light.position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }
    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

void main()
{
    fragment_color = vec4(CalculatePhongModel(position_in_view, normal_in_view), 1.0);
}
```

## 16.2 **Phong**着色渲染展示

![Phong着色渲染结果](./images/Phong着色渲染结果.gif)

[返回](../../README.md)