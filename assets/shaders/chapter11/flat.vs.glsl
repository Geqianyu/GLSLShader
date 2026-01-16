#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) flat out vec3 light_intensity;

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

void main()
{
    vec3 normal = normalize( u_normal_matrix * vertex_normal);
    vec4 position_in_view = u_view_model_matrix * vec4(vertex_position,1.0);
    
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(vec3(u_light.position_in_view - position_in_view));
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_light.Ld * u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position_in_view.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_light.Ls * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }

    light_intensity = ambient_color + diffuse_color + specular_color;

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}