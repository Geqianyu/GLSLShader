#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

layout (location = 0) out vec3 world_position;
layout (location = 1) out vec3 world_normal;

uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    world_position = vec3(u_model_matrix * vec4(vertex_position, 1.0));
    world_normal = normalize(u_normal_matrix * vertex_normal);

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}