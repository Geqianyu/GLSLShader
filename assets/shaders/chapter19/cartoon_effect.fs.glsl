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
} u_material;

const int levels = 3;
const float scale_factor = 1.0 / levels;

vec3 ToonShade()
{
    vec3 normal = normalize(normal_in_view);
    vec3 s = normalize(u_light.position_in_view.xyz - position_in_view);

    vec3 ambient_color = u_light.La * u_material.Ka;

    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * floor(s_dot_n * levels) * scale_factor;

    return ambient_color + u_light.L * diffuse_color;
}

void main()
{
    fragment_color = vec4(ToonShade(), 1.0);
}