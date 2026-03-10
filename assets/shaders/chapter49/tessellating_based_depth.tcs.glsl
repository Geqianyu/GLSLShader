#version 460

layout (vertices = 16) out;

uniform int u_min_tess_level;
uniform int u_max_tess_level;
uniform float u_max_depth;
uniform float u_min_depth;
uniform mat4 u_model_view_matrix;

void main()
{
    vec4 p = u_model_view_matrix * gl_in[gl_InvocationID].gl_Position;

    float depth = clamp((abs(p.z) - u_min_depth) / (u_max_depth - u_min_depth), 0.0, 1.0);

    float tess_level = mix(u_max_tess_level, u_min_tess_level, depth);

    gl_TessLevelOuter[0] = tess_level;
    gl_TessLevelOuter[1] = tess_level;
    gl_TessLevelOuter[2] = tess_level;
    gl_TessLevelOuter[3] = tess_level;

    gl_TessLevelInner[0] = tess_level;
    gl_TessLevelInner[1] = tess_level;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}