#version 460

layout (triangles_adjacency) in;

layout (triangle_strip, max_vertices = 15) out;

in vec3 view_position[];
in vec3 view_normal[];

out vec3 g_normal;
out vec3 g_position;
flat out int g_is_edge;

uniform float u_edge_width;
uniform float u_pct_extend;

bool IsFrontFace(vec3 a, vec3 b, vec3 c)
{
    return ((a.x * b.y - b.x * a.y) + (b.x * c.y - c.x * b.y) + (c.x * a.y - a.x * c.y)) > 0.0;
}

void EmitQuad(vec3 e0, vec3 e1)
{
    vec2 ext = u_pct_extend * (e1.xy - e0.xy);
    vec2 v = normalize(e1.xy - e0.xy);
    vec2 n = vec2(-v.y, v.x) * u_edge_width;

    g_is_edge = 1;

    gl_Position = vec4(e0.xy - ext, e0.z, 1.0);
    EmitVertex();
    gl_Position = vec4(e0.xy - n - ext, e0.z, 1.0);
    EmitVertex();
    gl_Position = vec4(e1.xy + ext, e1.z, 1.0);
    EmitVertex();
    gl_Position = vec4(e1.xy - n + ext, e1.z, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

    if (IsFrontFace(p0, p2, p4))
    {
        if (!IsFrontFace(p0, p1, p2)) EmitQuad(p0, p2);
        if (!IsFrontFace(p2, p3, p4)) EmitQuad(p2, p4);
        if (!IsFrontFace(p4, p5, p0)) EmitQuad(p4, p0);
    }

    g_is_edge = 0;

    g_normal = view_normal[0];
    g_position = view_position[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    g_normal = view_normal[2];
    g_position = view_position[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    g_normal = view_normal[4];
    g_position = view_position[4];
    gl_Position = gl_in[4].gl_Position;
    EmitVertex();

    EndPrimitive();
}