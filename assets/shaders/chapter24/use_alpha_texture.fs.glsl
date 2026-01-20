#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv_in_view;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_base_texture;
layout (binding = 1) uniform sampler2D u_alpha_texture;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ks;
    float shininess;
} u_material;

vec3 CalculateBlinnPhong(vec3 position, vec3 normal)
{
    vec3 base_color = texture(u_base_texture, uv_in_view).rgb;

    vec3 ambient_color = u_light.La * base_color;

    vec3 s = normalize(u_light.position_in_view.xyz - position_in_view);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = base_color * s_dot_n;

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
    vec4 alpha = texture(u_alpha_texture, uv_in_view);
    if (alpha.a < 0.15)
    {
        discard;
    }
    else
    {
        if (gl_FrontFacing)
        {
            fragment_color = vec4(CalculateBlinnPhong(position_in_view, normalize(normal_in_view)), 1.0);
        }
        else
        {
            fragment_color = vec4(CalculateBlinnPhong(position_in_view, -normalize(normal_in_view)), 1.0);
        }
    }
}