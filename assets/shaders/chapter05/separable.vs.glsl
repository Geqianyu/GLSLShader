#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

uniform vec3 u_color_mask = vec3(0.0);

layout (location = 0) out vec3 color;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

void main()
{
    color = vertex_color * u_color_mask;
    gl_Position = vec4(vertex_position, 1.0);
}