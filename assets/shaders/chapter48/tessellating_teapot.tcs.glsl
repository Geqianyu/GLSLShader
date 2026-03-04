#version 460

layout (vertices = 16) out;

uniform int u_tess_level;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = float(u_tess_level);
    gl_TessLevelOuter[1] = float(u_tess_level);
    gl_TessLevelOuter[2] = float(u_tess_level);
    gl_TessLevelOuter[3] = float(u_tess_level);

    gl_TessLevelInner[1] = float(u_tess_level);
    gl_TessLevelInner[0] = float(u_tess_level);
}