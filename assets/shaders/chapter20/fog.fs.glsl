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

uniform struct FogInfo
{
    float max_distance;
    float min_distance;
    vec3 color;
} u_fog;

vec3 CalculateBlinnPhong(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(u_light.position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }
    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

void main()
{
    float distance_in_view = abs(position_in_view.z);
    float fog_factor = (u_fog.max_distance - distance_in_view) / (u_fog.max_distance - u_fog.min_distance);
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    vec3 shade_color = CalculateBlinnPhong(position_in_view, normalize(normal_in_view));

    vec3 color = mix(u_fog.color, shade_color, fog_factor);
    fragment_color = vec4(color, 1.0);
}