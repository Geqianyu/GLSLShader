#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec4 view_position;
layout (location = 1) out vec3 view_normal;
layout (location = 2) out vec4 projector_uv;

uniform mat4 u_model_matrix;
uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;
uniform mat4 u_projector_matrix;

void main()
{
    vec4 position = vec4(vertex_position, 1.0);

    view_position = u_view_model_matrix * position;
    view_normal = u_normal_matrix * vertex_normal;
    projector_uv = u_projector_matrix * u_model_matrix * position;

    gl_Position = u_mvp_matrix * position;
}