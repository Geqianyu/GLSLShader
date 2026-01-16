#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 light_intensity;

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
    light_intensity = CalculateLightIntensity(normal, s, v, r);

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}