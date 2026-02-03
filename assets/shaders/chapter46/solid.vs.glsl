#version 460

layout (location = 0 ) in vec2 vertex_position;

uniform mat4 u_mvp_matrix;

void main()
{
    gl_Position = u_mvp_matrix * vec4(vertex_position, 0.0, 1.0);
}