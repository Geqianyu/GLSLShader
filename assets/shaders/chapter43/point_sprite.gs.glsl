#version 460

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

out vec2 uv;

uniform float u_size;
uniform mat4 u_projection_matrix;

void main()
{
    gl_Position = u_projection_matrix * (vec4(-u_size, -u_size, 0.0, 0.0) + gl_in[0].gl_Position);
    uv = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = u_projection_matrix * (vec4(u_size, -u_size, 0.0, 0.0) + gl_in[0].gl_Position);
    uv = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = u_projection_matrix * (vec4(-u_size, u_size, 0.0, 0.0) + gl_in[0].gl_Position);
    uv = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = u_projection_matrix * (vec4(u_size, u_size, 0.0, 0.0) + gl_in[0].gl_Position);
    uv = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}