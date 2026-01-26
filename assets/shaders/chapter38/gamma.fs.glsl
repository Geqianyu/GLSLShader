#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv_in_view;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
  vec4 position_in_view;
  vec3 intensity;
} u_light;

uniform struct MaterialInfo
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
} u_material;

uniform float u_gamma;

vec3 CalculatePhong(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.intensity * u_material.Ka;

    vec3 s = normalize(u_light.position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_light.intensity * u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if (s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_light.intensity * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }

    return ambient_color + diffuse_color + specular_color;
}

void main()
{
    vec3 color = CalculatePhong(position_in_view, normalize(normal_in_view));
    fragment_color = vec4(pow(color, vec3(1.0 / u_gamma)), 1.0);
}