# Chapter10 双面渲染

[返回](../../README.md)

在渲染一个完全封闭的网格时，多边形的背面是隐藏的，所以网格会被正确渲染。
但是，如果网格包含空洞时，背面的渲染就会出错，因为法线指向了相反的方向。
为了正确渲染这些背面，需要反转法线，并根据反转后的法线计算光照。

## 10.1 反转法线并进行光照计算

``` GLSL
#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 front_color;
layout (location = 1) out vec3 back_color;

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

void CalculateVectors(out vec3 normal, out vec3 s, out vec3 v, out vec3 r)
{
    normal = normalize(u_normal_matrix * vertex_normal);

    vec4 position_in_view = u_view_model_matrix * vec4(vertex_position, 1.0);
    s = normalize(vec3(u_light.position_in_view - position_in_view));
    v = normalize(-position_in_view.xyz);
    r = reflect(-s, normal);
}

vec3 CalculateLightIntensity(vec3 normal, vec3 s, vec3 v, vec3 r)
{
    vec3 ambient_color = u_light.La * u_material.Ka;
    vec3 diffuse_color = u_light.Ld * u_material.Kd * max(dot(normal, s), 0.0);
    vec3 specular_color = u_light.Ls * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    return ambient_color + diffuse_color + specular_color;
}

void main()
{
    vec3 normal;
    vec3 s;
    vec3 v;
    vec3 r;
    CalculateVectors(normal, s, v, r);

    front_color = CalculateLightIntensity(normal, s, v, r);
    back_color = CalculateLightIntensity(-normal, s, v, r);

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}
```

这段顶点着色器代码中，对光照进行了两次计算，分别是正向和反向的，`front_color = CalculateLightIntensity(normal, s, v, r);` `back_color = CalculateLightIntensity(-normal, s, v, r);` 。

## 10.2 根据正反面选择颜色

``` GLSL
#version 460

layout (location = 0) in vec3 front_color;
layout (location = 1) in vec3 back_color;

layout (location = 0) out vec4 fragment_color;

void main()
{
    if (gl_FrontFacing)
    {
        fragment_color = vec4(front_color, 1.0);
    }
    else
    {
        fragment_color = vec4(back_color, 1.0);
    }
}
```

这段片元着色器根据面片的正反面选择着色的颜色。

内置变量 `gl_FrontFacing` 可以判断当前面片是否是正面，判断方式是面片的环绕方式。
默认情况下，正向的多边形是在屏幕上以逆时针顺序显示的。
可以在 C++ 端通过 `glFrontFace` 进行设置。

[返回](../../README.md)