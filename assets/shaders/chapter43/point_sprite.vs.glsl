#version 460

layout (location = 0) in vec3 vertex_position;

uniform mat4 u_view_model_matrix;

void main()
{
    gl_Position = u_view_model_matrix * vec4(vertex_position, 1.0);
}