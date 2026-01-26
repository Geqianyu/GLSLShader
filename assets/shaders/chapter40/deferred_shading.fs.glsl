#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out vec3 position_data;
layout (location = 2) out vec3 normal_data;
layout (location = 3) out vec3 color_data;

uniform int u_pass;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform struct MaterialInfo
{
    vec3 Kd;
} u_material;

layout (binding = 0) uniform sampler2D u_position_texture;
layout (binding = 1) uniform sampler2D u_normal_texture;
layout (binding = 2) uniform sampler2D u_color_texture;

vec3 CalculateDiffuse(vec3 position, vec3 normal, vec3 diffuse)
{
    vec3 s = normalize(vec3(u_light.position_in_view) - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    return u_light.L * diffuse * s_dot_n;
}

void Pass1()
{
    position_data = position_in_view;
    normal_data = normalize(normal_in_view);
    color_data = u_material.Kd;
}

void Pass2()
{
    vec3 position = vec3(texture(u_position_texture, uv));
    vec3 normal = vec3(texture(u_normal_texture, uv));
    vec3 diffuse_color = vec3(texture(u_color_texture, uv));

    fragment_color = vec4(CalculateDiffuse(position, normal, diffuse_color), 1.0);
}

void main()
{
    if (u_pass == 1)
    {
        Pass1();
    }
    else if (u_pass == 2)
    {
        Pass2();
    }
}