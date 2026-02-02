#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 view_position;
layout (location = 1) out vec3 view_normal;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    view_position = (u_view_model_matrix * vec4(vertex_position, 1.0)).xyz;
    view_normal = u_normal_matrix * vertex_normal;
    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}