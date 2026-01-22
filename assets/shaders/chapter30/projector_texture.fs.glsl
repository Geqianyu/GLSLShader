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