#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

layout (location = 0) out vec3 color;

uniform mat4 u_model = mat4(1.0);

void main()
{
    color = vertex_color;
    gl_Position = u_model * vec4(vertex_position, 1.0);
}