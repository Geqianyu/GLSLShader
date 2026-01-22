# Chapter30 应用投影纹理

[返回](../../README.md)

将纹理应用到场景中的对象上，就好像纹理是从场景中某个位置的一个假想“幻灯机”投射出来的一样。
这种技术通常称为投影纹理映射。

要将纹理投射到表面上，需要做的就是根据表面位置与投影源(幻灯机)的相对位置来确定纹理坐标。
一种简单的方法是将投影仪视作位于场景中某处的相机。
定义一个以投影仪位置为中心的坐标系统，以及一个视图矩阵(**V**)，用于将坐标转换到投影仪的坐标系统。
接下来，定义一个透视投影矩阵(**P**)，它将视锥体转换为以原点为中心、大小为二的立方体区域。
将这两者结合起来，并再添加一个用于重新缩放和平移体积到大小为一、且中心位于（0.5, 0.5, 0.5）的体积的矩阵，我们就得到了以下变换矩阵:

$$
M =
\begin{bmatrix}
0.5 & 0   & 0   & 0.5\\
0   & 0.5 & 0   & 0.5\\
0   & 0   & 0.5 & 0.5\\
0   & 0   & 0   & 1
\end{bmatrix}
PV
$$

**注:** 有关该技术数学原理的更多详细信息，请查看由 NVIDIA 的 Cass Everitt 撰写的 [白皮书](https://www.nvidia.com/object/Projective_Texture_Mapping.html)。

## 30.1 编写顶点着色器

``` GLSL
#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec4 view_position;
layout (location = 1) out vec3 view_normal;
layout (location = 2) out vec4 projector_uv;

uniform mat4 u_model_matrix;
uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;
uniform mat4 u_projector_matrix;

void main()
{
    vec4 position = vec4(vertex_position, 1.0);

    view_position = u_view_model_matrix * position;
    view_normal = u_normal_matrix * vertex_normal;
    projector_uv = u_projector_matrix * u_model_matrix * position;

    gl_Position = u_mvp_matrix * position;
}
```

该顶点着色器中有一个 `u_projector_matrix` 变量，该变量由外部程序设置，是投影仪的矩阵。

## 30.2 编写片元着色器

``` GLSL
#version 460

layout (location = 0) in vec4 view_position;
layout (location = 1) in vec3 view_normal;
layout (location = 2) in vec4 projector_uv;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_projector_texture;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

vec3 CalculateBlinnPhong(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(u_light.position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if (s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }
    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

void main()
{
    vec3 color = CalculateBlinnPhong(view_position.xyz, normalize(view_normal));

    vec3 projector_texture_color = vec3(0.0);
    if(projector_uv.z > 0.0)
    {
        projector_texture_color = textureProj(u_projector_texture, projector_uv).rgb;
    }

    fragment_color = vec4(color + projector_texture_color * 0.5, 1);
}
```

这里用 `projector_uv` 来采样投影纹理，使用的采样函数是 `textureProj`。

**注:** 这里介绍的技术有一个很大的缺点。目前还不支持阴影，因此投影纹理会直接穿过场景中的任何物体，并出现在物体后面的地方（相对于投影仪而言）。

## 30.3 应用投影纹理渲染展示

![应用投影纹理](./images/应用投影纹理.gif)

[返回](../../README.md)