#version 460

layout (location = 0) in vec3 vertex_position;

layout (location = 0) out vec3 position;

uniform mat4 u_mvp_matrix;

void main()
{
    position = vertex_position;
    gl_Position = u_mvp_matrix * vec4(vertex_position,1.0);
}