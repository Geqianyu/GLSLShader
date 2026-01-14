#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 light_intensity;

uniform vec4 u_light_position_in_view;
uniform vec3 u_Kd;
uniform vec3 u_Ld;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    vec3 normal = normalize(u_normal_matrix * vertex_normal);
    vec4 position_in_view = u_view_model_matrix * vec4(vertex_position, 1.0);
    vec3 s = normalize(vec3(u_light_position_in_view - position_in_view));
    light_intensity = u_Ld * u_Kd * max(dot(s, normal), 0.0);
    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}