#version 460

layout (vertices = 4) out;

uniform int u_segments_num;
uniform int u_strips_num;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = float(u_strips_num);
    gl_TessLevelOuter[1] = float(u_segments_num);
}