#version 460

layout (vertices = 4) out;

uniform int u_outer;
uniform int u_inner;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = float(u_outer);
    gl_TessLevelOuter[1] = float(u_outer);
    gl_TessLevelOuter[2] = float(u_outer);
    gl_TessLevelOuter[3] = float(u_outer);

    gl_TessLevelInner[0] = float(u_inner);
    gl_TessLevelInner[1] = float(u_inner);
}