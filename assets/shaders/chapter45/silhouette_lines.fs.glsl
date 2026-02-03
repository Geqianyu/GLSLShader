#version 460

in vec3 g_normal;
in vec3 g_position;
flat in int g_is_edge;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
    vec4 position; 
    vec3 intensity;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

uniform vec4 u_line_color;

const int levels = 3;
const float scale_factor = 1.0 / levels;

vec3 CalculateToonShade()
{
    vec3 s = normalize(u_light.position.xyz - g_position.xyz);

    vec3 ambient_color = u_material.Ka;

    float cosine = dot(s, g_normal);

    vec3 diffuse_color = u_material.Kd * ceil(cosine * levels) * scale_factor;

    return u_light.intensity * (ambient_color + diffuse_color);
}

void main()
{
    if(g_is_edge == 1)
    {
        fragment_color = u_line_color;
    }
    else
    {
        fragment_color =  vec4(CalculateToonShade(), 1.0);
    }
}